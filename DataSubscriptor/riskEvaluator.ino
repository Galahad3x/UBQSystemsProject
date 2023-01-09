#include "healtStation.h"

#define VL 0
#define A 1
#define R 2
#define H 3


int heart_rate_level(int heartRate) {
  if (heartRate < 60) {
    return (int)VL;
  }

  if (heartRate >= 60 && heartRate <= 100) {
    return (int)A;
  }

  if (heartRate > 100 && heartRate <= 160) {
    return (int)R;
  }

  if (heartRate > 160) {
    return (int)H;
  }
}


RiskLevel risk_ok_moviment(int heartRate) {
  int level = heart_rate_level(heartRate);
  if (level == VL) {
    return ALERT_LEVEL;
  }
  return OK_LEVEL;
}


RiskLevel risk_bad_moviment(int heartRate) {
  int level = heart_rate_level(heartRate);
  if (level == VL) {
    return VERY_URGENT_LEVEL;
  } else if (level == A || level == R) {
    return WORRY_LEVEL;
  }
  return ALERT_LEVEL;
}


void calculate_risk(int heartRate, bool okMoviment) {
  struct StationMessage stationMessage;
  RiskLevel risk;
  if (okMoviment) {
    risk = risk_ok_moviment(heartRate);
  } else {
    risk = risk_bad_moviment(heartRate);
  }
  // send results to the station
  stationMessage.heartRate = heartRate;
  stationMessage.risk = risk;
  stationMessage.moviment = (char *)(okMoviment ? "g" : "b");
  Serial.println("Send the results to the station");
  xQueueSend(stationQueue, &stationMessage, (TickType_t)0);
}

void risk_evaluator(void *taskArgs) {
  struct RiskEvaluatorMessage message;
  struct StationMessage stationMessage;

  int heartRate = -1;
  char *movimentStatus = NULL;
  long lastBeat = millis();
  bool update = false;
  while (1) {
    long delta = millis() - lastBeat;
    // if have new data update the information
    if (xQueueReceive(riskEvaluatorQueue, &message, (TickType_t)10) == pdTRUE) {
      // update risk evaluator data
      if (message.biometricDataType == HEART_RATE) {
        Serial.print("Arrive heart of rate value=");
        Serial.println(message.heartRate);
        heartRate = (heartRate == 0 || heartRate == -1
                       ? message.heartRate
                       : (int)(0.8 * message.heartRate + 0.2 * heartRate));  // weight mean
        stationMessage.heartRate = heartRate;
        stationMessage.risk = NOT_LEVEL;
        stationMessage.moviment = NULL;
        update = true;
      } else if (message.biometricDataType == MOTION) {
        stationMessage.heartRate = heartRate;
        stationMessage.risk = NOT_LEVEL;
        stationMessage.moviment = NULL;
        movimentStatus = message.movimentStatus;
        update = true;
      }    
      // if have updated data calculate risk
      if (heartRate != -1 && movimentStatus != NULL) {
        bool isOkMoviment = *movimentStatus == 'g';
        calculate_risk(heartRate, isOkMoviment);
        // reset moviment status data
        movimentStatus = NULL;
        update=false; // reset status
        lastBeat = millis();
      } 
    }
    if(update && delta > 2000) {
      // send update info
      xQueueSend(stationQueue, &stationMessage, (TickType_t)0);
      update = false;
      lastBeat = millis();
    }
    vTaskDelay(800 * xTicksFactor);
  }
}