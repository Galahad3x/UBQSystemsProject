#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>

#include "healtStation.h"
#include <string.h>

const char* hrTOPIC = "data/heartRate";
const char* mvTOPIC = "data/moviment";

int heartRate = 0;

void process_hr_message(byte* payload, unsigned int length) {
  struct RiskEvaluatorMessage message = {HEART_RATE, -1, NULL };
  // cast message to int
  int value = 0;
  for (int i = 0; i < length; i++) {
    value += (int)(payload[i] - '0') * pow(10, length - i - 1);
  }
  // send data to the risk evaluator
  Serial.print("");
  message.heartRate = value;
  xQueueSend(riskEvaluatorQueue, &message, (TickType_t)0);
}


void process_moviment_message(byte* payload, unsigned int length) {
  struct RiskEvaluatorMessage message = {MOTION, -1, NULL };
  // ... //
  Serial.print("Motion=");
  Serial.println((char)payload[0]);
  message.movimentStatus = (char *)payload;
  xQueueSend(riskEvaluatorQueue, &message, (TickType_t)0);
}

void subscriberCallback(char* topic, byte* payload, unsigned int length) {
  // at the moment unique topic
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  if (String(topic) == hrTOPIC) {
    process_hr_message(payload, length);
  } else {
    process_moviment_message(payload, length);
  }
}

void reconnect() {
  while (!client.connected()) {
    String client_id = "ESP01-dataPublisher1-";
    client_id += String(random(0xffff), HEX);
    Serial.println("Try to connect to mqtt broker");
    if (client.connect(client_id.c_str())) {
      client.subscribe(hrTOPIC);
      Serial.print("Client connected to the topic data/heartRate");
      Serial.println(hrTOPIC);
      Serial.print("Client connected to the topic data/moviment");
      Serial.println(mvTOPIC);
      client.subscribe(mvTOPIC);
    } else {
      Serial.println("Error to connect mqtt");
      vTaskDelay(5000);
    }
  }
}

// data subscriber task definition
void data_subscriber(void* taskArgs) {
  while (1) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    vTaskDelay(600);
  }
}