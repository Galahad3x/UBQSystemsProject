#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "healtStation.h"


void init_display() {
  lcd.init();
  lcd.home();
  lcd.backlight();
  lcd.print("Waiting for data");
}


void init_servo() {
   // init servo
  servo.attach(13);
}

void update_servo(RiskLevel risk) {
  int value = risk * 45;
  //long degrees = (int)map((long)value, 0, 120, 0, 180);
  int degrees = value;
  Serial.print("Rotate servo ");
  Serial.println(degrees);
  servo.write(degrees);
}

void display(int value) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BPM=");
  lcd.print(value);
  lcd.setCursor(0, 0);
}


void output_station(void* taskArgs) {
  struct StationMessage dataMessage;
  while(1) {
      if(xQueueReceive(stationQueue, &dataMessage, (TickType_t )10) == pdTRUE) {
        Serial.print("Print heart rate value=");
        Serial.println(dataMessage.heartRate);
        display(dataMessage.heartRate);
        update_servo(dataMessage.risk);
      }
      delay(1000);
  }

}





// #include <WiFi.h>
// #include <WebServer.h>
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// #include <PubSubClient.h>
// #include <cstring>

// struct StationMessage {
//   int heartRate;
// };


// // const char* ssid  = "Test";
// // const char* password  = "Test1234";
// const char* ssid = "alex";
// const char* pass = "ar6mKy3kmertUPO2134ZxD5";


// // mqtt config params
// const char* mqtt_server = "172.20.10.3";
// //const char* mqtt_server = "test.mosquitto.org";
// const int port = 1883;
// const char* topic = "data/heartRate";


// // clients
// WiFiClient espClient;
// PubSubClient client(espClient);
// LiquidCrystal_I2C lcd(0x27, 20, 4);
// int current_value = 0;


// // mesage queues
// QueueHandle_t stationQueue;



// void callback(char* topic, byte* payload, unsigned int length) {
//   int value = 0;
//   struct StationMessage message;
//   Serial.print("Message arrived [");
//   Serial.print(topic);
//   Serial.println("] ");
//   for (int i = 0; i < length; i++) {
//     //Serial.print((char)payload[i]);
//     value += (int)(payload[i] - '0') * pow(10, length - i - 1);
//   }
//   //current_value = atoi((char*)payload);
//   //update_display();
//   message.heartRate = value;
//   xQueueSend(stationQueue, &message, ( TickType_t )0);
// }


// void init_display() {
//   lcd.init();
//   lcd.home();
//   lcd.backlight();
//   lcd.print("Waiting for data");
// }

// void update_display() {
//   Serial.print("Current value ");
//   Serial.println(current_value);
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("BPM=");
//   lcd.print(current_value);
//   lcd.setCursor(0, 0);
// }

// void display(int value) {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("BPM=");
//   lcd.print(value);
//   lcd.setCursor(0, 0);
// }


// void output_station(void* taskArgs) {
//   struct StationMessage dataMessage;
//   while(1) {
//       if(xQueueReceive(stationQueue, &dataMessage, (TickType_t )10) == pdTRUE) {
//         Serial.print("Print heart rate value=");
//         Serial.println(dataMessage.heartRate);
//         display(dataMessage.heartRate);
//       }
//   }
// }


// void reconnect() {
//   while (!client.connected()) {
//     String client_id = "ESP01-dataPublisher1-";
//     client_id += String(random(0xffff), HEX);
//     Serial.println("Try to connect to mqtt broker");
//     if (client.connect(client_id.c_str())) {
//       client.subscribe(topic);
//       Serial.print("Client connected to the topic");
//       Serial.println(topic);

//     } else {
//       Serial.println("Error to connect mqtt");
//       delay(5000);
//     }
//   }
// }

// void data_subscriber(void* taskArgs) {
//   while (1) {
//     if (!client.connected()) {
//       reconnect();
//     }
//     client.loop();
//     delay(10);
//   }
// }

// void setup_wifi() {
//   delay(1000);
//   // We start by connecting to a WiFi network
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);

//   WiFi.begin(ssid, pass);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
// }

// void setup() {
//   Serial.begin(115200);
//   delay(1000);  
//   // init the display
//   init_display();
//   setup_wifi();
//   // setup mqtt
//   client.setServer(mqtt_server, port);
//   client.setCallback(callback);

//   // queues creation
//   stationQueue = xQueueCreate( 10, sizeof(struct StationMessage));

//   // start data subscriber task
//   xTaskCreatePinnedToCore(data_subscriber, "data_subscriber", 2048, NULL, 1, NULL, 1);
//   xTaskCreatePinnedToCore(output_station, "output_station", 2048, NULL, 1, NULL, 0);
// }

// void loop() {}