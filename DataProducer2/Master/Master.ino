// CODI DEL ESP-01

#include <ESP8266WiFi.h>
#include <Wire.h>

#define SSID "Slippin Wifi"
#define PWD "bettercallsaul"
#define I2C_SPEED_FAST 400000

const char* ssid = SSID;
const char* password = PWD;

const char* host = "192.168.42.64";
const uint16_t port = 9090;

void setup() {
  Serial.begin(9600); /* begin serial for debug */
  Wire.begin(0,2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */

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

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }
}

void loop() {
  /*
  Wire.beginTransmission(8); // begin with device address 8
  Wire.write("Hello Arduino");  // sends hello string
  Wire.endTransmission();    // stop transmitting
  */

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  
  Wire.requestFrom(8, 13); // request & read data of size 13 from slave
  while(Wire.available()){
    char c = Wire.read();
    Serial.print(c);
    if (client.connected()) {
      client.print(c);     
    }
  }
  Serial.println();
  if (client.connected()) {
    client.println();     
  }
  client.stop();
  delay(3000);
}
