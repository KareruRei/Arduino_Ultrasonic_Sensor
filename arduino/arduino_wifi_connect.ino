#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
char serverAddress[] = SECRET_WS_SERVER;
int port = SECRET_WS_PORT;

WiFiClient wifi;
WebSocketClient client = WebSocketClient(wifi, serverAddress, port);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nWiFi Connected! IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.println("Starting WebSocket connection...");
  client.begin(); // Initiates the handshake

  while (client.connected()) {
    // 1. Check for incoming messages
    int messageSize = client.parseMessage();
    if (messageSize > 0) {
      Serial.print("Received: ");
      Serial.println(client.readString());
    }

    // 2. Send data (Example: Heartbeat every 10 seconds)
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 10000) {
      client.beginMessage(TYPE_TEXT);
      client.print("R4 Hello!");
      client.endMessage();
      lastSend = millis();
    }
  }

  Serial.println("Disconnected. Retrying in 5 seconds...");
  delay(5000);
}