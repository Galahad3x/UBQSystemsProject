#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "healtStation.h"

char *RISKS[4] = {"OK", "WORRY","ALERT","URGENT"};

void init_display() {
  lcd.init();
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
  // limit angles to reduce times the servo blocks in 0 or 180
  long degrees = (int)map((long)value, 0, 180, 10, 170); 
  Serial.print("Rotate servo ");
  Serial.println(degrees);
  servo.write(degrees);
  vTaskDelay(250 * xTicksFactor);
}

void display(int value, char *moviment, RiskLevel risk) {
  lcd.clear();
  lcd.printf("BPM=%d", value);
  lcd.setCursor(4, 0);
  lcd.print(value);
  lcd.setCursor(0,1);
  lcd.printf("MV=%s", moviment);
  if (risk != NOT_LEVEL) {
    lcd.printf(",Risk=%s", RISKS[(int)risk]);
  }
  vTaskDelay(500 * xTicksFactor);
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
      char *moviment = (char *)(dataMessage.risk == NOT_LEVEL ? "-" : dataMessage.moviment);
      Serial.print("Moviment=");
      Serial.println(moviment);
      display(dataMessage.heartRate, moviment, dataMessage.risk);
    }
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}