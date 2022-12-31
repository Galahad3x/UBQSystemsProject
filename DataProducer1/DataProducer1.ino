#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <PubSubClient.h>

#include "MAX30105.h"
#include "heartRate.h"

#define SSID "Slippin Wifi"
#define PWD "bettercallsaul"
#define I2C_SPEED_FAST 400000

const bool TESTMODE = false;




MAX30105 particleSensor;
const byte RATE_SIZE = 4; 
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0;

float beatsPerMinute;
int beatAvg;

// wifi config params
const char* ssid = SSID;
const char* pass = PWD;

// mqtt config params
const char* mqtt_server = "test.mosquitto.org";
const int port = 1883;
const char* topic = "data/heartRate";

// data send config params
unsigned int value = 0;
unsigned long lastTime = 0;
unsigned long timerDelay = 5000; // Timeer 5 seconds


// clients
WiFiClient espClient;
PubSubClient client(espClient);

void send_value(int value) {
  Serial.println(WiFi.localIP());
  Serial.print("Publish message  ");
  Serial.println(value);
  if (!client.connected()) {
    Serial.println("Error mqtt conexion \n");
  } else {
      Serial.println(String("Sending " + String(value)).c_str());
      client.publish(topic, String(value).c_str());
  }

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);  
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup_wifi() {
  delay(1000);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    String client_id = "ESP32-dataSubscriber";
    client_id += String(random(0xffff), HEX);
    Serial.println("Try to connect to mqtt broker");
    if(client.connect(client_id.c_str())) {
      Serial.println("Client connected");
      client.publish("data/heartRate", String(-1).c_str());
    } else{
      Serial.println("Error to connect mqtt");
      delay(5000);
    }
  }
}


void publish(int beatsPerMinute) {
  Serial.println(WiFi.localIP());
  Serial.print("Publish message  ");
  Serial.println(beatsPerMinute);
  if (!client.connected()) {
    Serial.println("Error mqtt conexion \n");
  } else {
    Serial.println(String("Sending " + String(beatsPerMinute)).c_str());
    client.publish(topic, String(beatsPerMinute).c_str());
  }
}

void setup() {
  Serial.begin(115200); 

  Wire.begin(0, 2);

  Serial.print("Intentant connectar a ");
  Serial.println(ssid);

  //init_display();
  setup_wifi();
  // setup mqtt
  client.setServer(mqtt_server, port);
  client.setCallback(callback);

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

  delay(100); // execute once every 5 minutes, don't flood remote service
}
