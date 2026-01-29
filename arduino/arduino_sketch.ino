#include <stdint.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <Servo.h>
#include "arduino_secrets.h"

// ===== WIFI & WEBSOCKET CONFIG =====
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char wsServer[] = SECRET_WS_SERVER;
int wsPort = SECRET_WS_PORT;

WiFiClient wifi;
WebSocketClient ws(wifi, wsServer, wsPort);

Servo radar;

const byte red = 4;
const byte blue = 5;
const byte green = 6;

const byte buzzer = 2;
const byte trigPin = 9;
const byte echoPin = 10;

long duration;
uint16_t distance;

int angle = 0;
bool locked = false, moveRight = true;

unsigned long lastWsAttempt = 0;
const unsigned long wsReconnectInterval = 3000;

void setup() {
  radar.attach(11);

  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(115200);
  while (!Serial);

  // ===== CONNECT TO WIFI =====
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // ===== CONNECT TO WEBSOCKET =====
  Serial.println("Connecting to WebSocket...");
  ws.begin();
  ws.beginMessage(TYPE_TEXT);
  ws.print("{\"role\":\"arduino\"}");
  ws.endMessage();
}


void loop() {
  autoReconnectWebSocket();

  // ===== RADAR SWEEP MODE =====
  if (!locked) {
    for (; (moveRight && angle <= 180) || (!moveRight && angle >= 0); angle = moveRight ? angle+3 : angle-3) {
      radar.write(angle);
      delay(15);

      distance = getDistance();
      sendRadarData(angle, distance, locked);

      if (distance < 50 distance > 0) {
        locked = true;
        break;
      }
    }

    if (moveRight) {
      if (angle > 180) {
        angle = 180;
        moveRight = false;
      }
    }
    else if (angle < 0) {
      angle = 0;
      moveRight = true;
    }
  }
  else {
    radar.write(angle);

    digitalWrite(red, HIGH);
    digitalWrite(blue, HIGH);
    digitalWrite(green, HIGH);

    tone(buzzer, 1000, 120);
    delay(120);

    distance = getDistance();

    if (distance >= 50 || distance == 0) {
      locked = false;
      noTone(buzzer);

      digitalWrite(red, LOW);
      digitalWrite(blue, LOW);
      digitalWrite(green, LOW);

      return;
    }

    Serial.println(distance);
    sendRadarData(angle, distance, locked);
  }
}


void sendRadarData(uint8_t angle, uint16_t distance, bool locked) {
  if (!ws.connected()) return;

  uint8_t buffer[4] = {angle, distance >> 8, distance & 0xFF, locked};
  ws.beginMessage(TYPE_BINARY);
  ws.write(buffer, 4);
  ws.endMessage();
}

void autoReconnectWebSocket() {
  if (ws.connected() || millis() - lastWsAttempt <= wsReconnectInterval) return;

  Serial.println("WebSocket disconnected. Reconnecting...");
  ws.begin();
  ws.beginMessage(TYPE_TEXT);
  ws.print("{\"role\":\"arduino\"}");
  ws.endMessage();
  lastWsAttempt = millis();
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 25000);
  if (duration == 0) return 0;

  return duration * 0.0343 / 2;
}
