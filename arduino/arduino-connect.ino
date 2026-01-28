#include <stdint.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <Servo.h>

// ===== WIFI & WEBSOCKET CONFIG =====
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

char wsServer[] = "192.168.50.179";   // backend IP / domain
int wsPort = 8080;                   // websocket port

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

bool locked = false;
int lockAngle = 0;

void setup() {
  radar.attach(3);

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

void sendRadarData(uint8_t angle, uint16_t distance, bool locked) {
  if (!ws.connected()) return;

  uint8_t buffer[4] = {angle, distance >> 8, distance & 0xFF, locked};
  ws.beginMessage(TYPE_BINARY);
  ws.write(buffer, 4);
  ws.endMessage();
}

void loop() {
  // ===== AUTO-RECONNECT WEBSOCKET =====
  if (!ws.connected()) {
    Serial.println("WebSocket disconnected. Reconnecting...");
    ws.begin();
    
    ws.beginMessage(TYPE_TEXT);
    ws.print("{\"role\":\"arduino\"}");
    ws.endMessage();

    delay(1000);
    return;
  }

  // ===== RADAR SWEEP MODE =====
  if (!locked) {
    for (uint8_t angle = 0; angle <= 180; ++angle) {
      radar.write(angle);
      delay(15);

      distance = getDistance();
      sendRadarData(angle, distance, locked);

      if (distance > 0 && distance < 50) {
        locked = true;
        lockAngle = angle;
        break;
      }
    }

    for (int angle = 180; angle >= 0 && !locked; angle--) {
      radar.write(angle);
      delay(15);

      distance = getDistance();
      sendRadarData(angle, distance, locked);

      if (distance > 0 && distance < 50) {
        locked = true;
        lockAngle = angle;
        break;
      }
    }
  }

  // ===== LOCK MODE =====
  else {
    radar.write(lockAngle);

    digitalWrite(red, HIGH);
    digitalWrite(blue, HIGH);
    digitalWrite(green, HIGH);

    tone(buzzer, 1000);
    delay(120);
    noTone(buzzer);
    delay(120);

    distance = getDistance();
    sendRadarData(lockAngle, distance, locked);

    if (distance >= 50 || distance == 0) {
      locked = false;
      noTone(buzzer);

      digitalWrite(red, LOW);
      digitalWrite(blue, LOW);
      digitalWrite(green, LOW);
    }
  }
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
