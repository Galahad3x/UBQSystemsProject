#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <cstring>
#include <Servo.h>

#define SERVO_PORT 0


#ifndef HEALT_STATION
#define HEALT_STATION


enum BiometricDataType{
  HEART_RATE,
  MOTION,
};

enum RiskLevel{
  OK_LEVEL,
  WORRY_LEVEL,
  ALERT_LEVEL,
  VERY_URGENT_LEVEL,
  NOT_LEVEL,
};


// messages types definitions
struct StationMessage {
  int heartRate;
  RiskLevel risk;
};

struct RiskEvaluatorMessage {
  BiometricDataType biometricDataType;
  int heartRate;
  char *movimentStatus;
};


// clients
WiFiClient espClient;
PubSubClient client(espClient);

// display object creation
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo servo;


// mesage queues definitions
QueueHandle_t stationQueue;
QueueHandle_t riskEvaluatorQueue;

// semaphores definitions
SemaphoreHandle_t RDataInSemaphore;
#endif