#include <LiquidCrystal_I2C.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>

const char* ssid = "ComLab506";
const char* password = "#Ramswifi";
const char* serverAddress = "http://localhost/arduino-api";
const int port = 80;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);

void setup()
{
    
}

void loop()
{

}