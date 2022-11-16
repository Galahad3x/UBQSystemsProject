#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>

#include "healtStation.h"


const char* topic = "data/heartRate";

void subscriberCallback(char* topic, byte* payload, unsigned int length) {
  int value = 0;
  //struct StationMessage message;
  struct RiskEvaluatorMessage message;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    value += (int)(payload[i] - '0') * pow(10, length - i - 1);
  }
  //current_value = atoi((char*)payload);
  //update_display();
  message.heartRate = value;
  //xQueueSend(stationQueue, &message, ( TickType_t )0);
  xQueueSend(riskEvaluatorQueue, &message, ( TickType_t )0);
}


void reconnect() {
  while (!client.connected()) {
    String client_id = "ESP01-dataPublisher1-";
    client_id += String(random(0xffff), HEX);
    Serial.println("Try to connect to mqtt broker");
    if (client.connect(client_id.c_str())) {
      client.subscribe(topic);
      Serial.print("Client connected to the topic");
      Serial.println(topic);

    } else {
      Serial.println("Error to connect mqtt");
      delay(5000);
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
    delay(10);
  }
}
