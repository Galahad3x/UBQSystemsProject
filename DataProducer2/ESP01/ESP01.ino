// CODI DEL ESP-01

#include <ESP8266WiFi.h>
#include <Wire.h>

#define SSID "Slippin Wifi"
#define PWD "bettercallsaul"
#define I2C_SPEED_FAST 400000

const char* ssid = SSID;
const char* password = PWD;

const char* host = "192.168.212.64";
const uint16_t port = 9090;

// topics definitions
const char* mvTOPIC = "data/moviment";
const char* messageTopic = "message/moviment";

// Mqtt client definition
WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    String client_id = "ESP01-dataPubliser1-";
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);  
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void publish(char movimentType) {
  Serial.println(WiFi.localIP());
  Serial.print("Publish message  ");
  Serial.println(beatsPerMinute);
  if (!client.connected()) {
    Serial.println("Error mqtt conexion \n");
  } else {
    Serial.println(String("Sending " + String(movimentType)).c_str());
    client.publish(topic, String(movimentType).c_str());
  }
}


void messageMQTT(char *message) {
  Serial.println(WiFi.localIP());
  Serial.print("Publish message  ");
  Serial.println(message);
  if (!client.connected()) {
    Serial.println("Error mqtt conexion \n");
    reconnect();
  } else {
    client.publish(messageTopic, message);
  }
}

void setup_wifi() {
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

void setup() {
  Serial.begin(9600); /* begin serial for debug */
  Wire.begin(0, 2);                /* join i2c bus with address 8 */
  setup_wifi();
  // WiFiClient client;
  // if (!client.connect(host, port)) {
  //   Serial.println("connection failed");
  //   delay(5000);
  //   return;
  // }
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}


void loop() {
  Wire.requestFrom(8, 1);
  while(Wire.available()){
    char c = Wire.read();
    Serial.print("Moviment: ")
    Serial.println(c);
    if (c != 't') {
      publish(c);
    }
  }
}

// void loop() {
//   WiFiClient client;
//   if (!client.connect(host, port)) {
//     Serial.println("connection failed");
//     delay(5000);
//     return;
//   }
//   Wire.requestFrom(8, 1);
//   while(Wire.available()){
//     char c = Wire.read();
//     Serial.print(c);
//     if (client.connected()) {
//       client.print(c);     
//     }
//   }
//   Serial.println();
//   if (client.connected()) {
//     client.println();     
//   }
//   client.stop();
//   delay(100);
// }
