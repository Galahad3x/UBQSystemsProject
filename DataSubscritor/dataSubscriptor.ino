#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


const char* ssid  = "Test";
const char* password  = "Test1234";


WebServer server(80);
LiquidCrystal_I2C lcd(0x27,20,4); 
int current_value=0;

void handleConexion() {
  Serial.println("New conexion");
  // update the display value if 
  if (server.hasArg("value")) {
    current_value = server.arg("value").toInt();
    server.send(200, "text/html", "<h1>ok</h1>");
    update_display();
  } else {
    server.send(400, "text/html", "<h1>Bad request</h1>");
  }
}

void init_display() {
  lcd.init();
  lcd.home();
  lcd.backlight();
  lcd.print("Waiting for data");
}

void update_display() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BPM=");
  lcd.print(current_value);
  lcd.setCursor(0,1);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

   // Create wifi access point 
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    // Create server
    server.begin();
    Serial.println("Server started");

    server.on("/", HTTP_POST, handleConexion);

    // init the display
    init_display();
}

void loop() {
  server.handleClient();
}
  