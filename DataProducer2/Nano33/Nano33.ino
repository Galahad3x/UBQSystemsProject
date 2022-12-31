#include <Wire.h>

void setup() {
  
  Wire.begin(8); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  
  Serial.begin(115200);           /* start serial for debug */
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void receiveEvent(int howMany){
  while (0 < Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
  }
  Serial.println();
}

void requestEvent(){
  Wire.write("G");
}

void loop() {
  delay(2000);
}