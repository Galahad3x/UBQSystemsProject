#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "healtStation.h"

// Make custom characters:
byte Heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

void init_display() {
  lcd.init();
  // create custom characters
  lcd.createChar(0, Heart);
  // start windows content
  lcd.home();
  lcd.backlight();
  lcd.print("Waiting for data");
}


void init_servo() {
  servo.attach(SERVO_PORT);
}

void update_servo(RiskLevel risk) {
  int value = risk * 60;
  long degrees = (int)map((long)value, 0, 180, 10, 170);
  Serial.print("Rotate servo ");
  Serial.println(degrees);
  servo.write(degrees);
}

void display(int value, char *moviment) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BPM=");
  lcd.print(value);
  lcd.setCursor(0,1);
  lcd.print("Motion=");
  lcd.print(moviment);
  lcd.setCursor(0, 0);
}


void output_station(void* taskArgs) {
  struct StationMessage dataMessage;
  while (1) {
    if (xQueueReceive(stationQueue, &dataMessage, (TickType_t)10) == pdTRUE) {
      Serial.print("Print heart rate value=");
      Serial.println(dataMessage.heartRate);
      if (dataMessage.risk != NOT_LEVEL) {
        Serial.print("Risk=");
        Serial.println(dataMessage.risk);
        update_servo(dataMessage.risk);
      }
      display(dataMessage.heartRate, "g");
    }
    vTaskDelay(4000);
  }
}