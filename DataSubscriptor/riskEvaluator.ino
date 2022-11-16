#include "healtStation.h"

void risk_evaluator(void* taskArgs) {
  struct RiskEvaluatorMessage message;
  struct StationMessage stationMessage;

  RiskLevel risk;
  int heartRate;
  while (1) {
    if(xQueueReceive(riskEvaluatorQueue, &message, (TickType_t )10) == pdTRUE) {
        Serial.print("Print heart rate value=");
        Serial.println(message.heartRate);
        heartRate = message.heartRate;
        // Evaluate risk(provisional)
        if (heartRate >= 180) {
          risk = VERY_URGENT_LEVEL;
        } else if (heartRate > 135) {
          risk = ALERT_LEVEL;
        } else if (heartRate > 90) {
          risk = WORRY_LEVEL;
        } else {
          risk = OK_LEVEL;
        }
        // send results to the station
        stationMessage.heartRate = heartRate;
        stationMessage.risk = risk;
        Serial.println("Send the results to the station");
        xQueueSend(stationQueue, &stationMessage, ( TickType_t )0);
      }
    delay(1000);
  }
}
