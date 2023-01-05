#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <cstring>

#include "healtStation.h"


// const char* ssid  = "Test";
// const char* password  = "Test1234";
// const char* ssid = "alex";
// const char* pass = "ar6mKy3kmertUPO2134ZxD5";
const char* ssid = "MOVISTAR_6910";
const char* pass = "SM8q9gwWF8XGpz78dFUB";


// mqtt config params
// const char* mqtt_server = "172.20.10.3";
//const char* mqtt_server = "192.168.1.37";
const char* mqtt_server = "test.mosquitto.org";
const int port = 1883;

void setup_wifi() {
  delay(1000);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // start wifi conexion
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

void setup() {
  Serial.begin(115200);
  delay(1000);  
  // init the display
  init_display();
  setup_wifi();
  // setup mqtt
  client.setServer(mqtt_server, port);
  client.setCallback(subscriberCallback);

  init_servo();

  // queues creation
  stationQueue = xQueueCreate(10, sizeof(struct StationMessage));
  riskEvaluatorQueue = xQueueCreate(10, sizeof(struct RiskEvaluatorMessage));

  // semaphores creation
  RDataInSemaphore = xSemaphoreCreateBinary();

  // start data subscriber task
  xTaskCreatePinnedToCore(data_subscriber, "data_subscriber", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(output_station, "output_station", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(risk_evaluator, "risk_evaluator", 2048, NULL, 1, NULL, 0);
}

void loop() {}