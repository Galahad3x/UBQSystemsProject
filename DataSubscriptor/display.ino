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
  servo.attach(SERVO_PORT);
}

void update_servo(RiskLevel risk) {
  int value = risk * 45;
  long degrees = (int)map((long)value, 0, 120, 10, 150);
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
      delay(2000);
  }

}