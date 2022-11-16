#include <ESP8266WiFi.h>
#include <Wire.h>

#include "MAX30105.h"
#include "heartRate.h"

#define SSID "Slippin Wifi"
#define PWD "bettercallsaul"

const char* ssid = SSID;
const char* password = PWD;

const char* host = "192.168.42.64";
const uint16_t port = 9090;

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

void setup() {
  Serial.begin(115200); 

  Wire.begin(2,0);

  Serial.print("Intentant connectar a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  Serial.println("sending data to server");
  if (client.connected()) {
    client.println("Kid named Arduino");
  }

  Serial.println();
  Serial.println("closing connection");
  client.stop();

  delay(100); // execute once every 5 minutes, don't flood remote service
}
