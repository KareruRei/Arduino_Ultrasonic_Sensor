#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <Servo.h>
// ===== WIFI & WEBSOCKET CONFIG =====
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

char wsServer[] = "192.168.1.100";   // backend IP / domain
int wsPort = 8080;                   // websocket port

WiFiClient wifi;
WebSocketClient ws(wifi, wsServer, wsPort);

Servo radar;

const int red = 4;
const int blue = 5;
const int green = 6;

const int buzzer = 2;

const int trigPin = 9;
const int echoPin = 10;

long duration;
int distance;

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

void sendRadarData(int angle, int distance, bool locked) {
  if (!ws.connected()) return;

  ws.beginMessage(TYPE_TEXT);
  ws.print("{\"angle\":");
  ws.print(angle);
  ws.print(",\"distance\":");
  ws.print(distance);
  ws.print(",\"locked\":");
  ws.print(locked ? "true" : "false");
  ws.print("}");
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
    for (int angle = 0; angle <= 180; angle++) {
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
