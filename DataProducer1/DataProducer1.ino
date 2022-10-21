#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>

#include "MAX30105.h"
#include "heartRate.h"

#define SSID "Test"
#define PWD "Test1234"

const bool TESTMODE = false;

const char* ssid = SSID;
const char* password = PWD;
const char* host = "192.168.42.64";
const uint16_t port = 9090;
const char* serverName = "http://192.168.4.1/"; // subscriber server

MAX30105 particleSensor;
const byte RATE_SIZE = 4; 
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0;

float beatsPerMinute;
int beatAvg;

void publish(int beatsPerMinute) {
  if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "value=";
      httpRequestData += beatsPerMinute;   

      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
}

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

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop() {
  // Use WiFiClient class to create TCP connections
  // Calculate BPM
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue) == true) {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    } 

    publish(beatsPerMinute);
  }

  if (TESTMODE)  {
      WiFiClient client;
      if (TESTMODE && !client.connect(host, port)) {
          Serial.println("connection failed");
          delay(5000);
          return;
      }
    if (client.connected()) {
      Serial.println("sending data to server");
      client.print("IR=");
      client.print(irValue);
      client.print(", BPM=");
      client.print(beatsPerMinute);
      client.print(", Avg BPM=");
      client.print(beatAvg);

      if (irValue < 50000)
        client.print(" No finger?");

      client.println();
    }

    Serial.println();
    Serial.println("closing connection");
    client.stop();

  }

  delay(100); // execute once every 5 minutes, don't flood remote service
}
