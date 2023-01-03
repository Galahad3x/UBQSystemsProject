#include <Wire.h>

#include <ES-DataProducer2_inferencing.h>
#include <Arduino_LSM9DS1.h>

#define CONVERT_G_TO_MS2    9.80665f
#define MAX_ACCEPTED_RANGE  2.0f

static bool debug_nn = false;

char value = 'c';

void setup() {
  
  Wire.begin(8); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  
  Serial.begin(115200);           /* start serial for debug */
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!\r\n");
  } else {
    Serial.println("IMU initialized\r\n");
  }

  if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 3) {
    Serial.println("ERR: EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME should be equal to 3 (the 3 sensor axes)\n");
    return;
  }
}

float ei_get_sign(float number) {
    return (number >= 0.0) ? 1.0 : -1.0;
}

void receiveEvent(int howMany){
  while (0 < Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
  }
  Serial.println();
}

void requestEvent(){
  Wire.write(value);
}

void loop() {
    // Allocate a buffer here for the values we'll read from the IMU
    float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = { 0 };

    for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 3) {
        // Determine the next tick (and then sleep later)
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        IMU.readAcceleration(buffer[ix], buffer[ix + 1], buffer[ix + 2]);

        for (int i = 0; i < 3; i++) {
            if (fabs(buffer[ix + i]) > MAX_ACCEPTED_RANGE) {
                buffer[ix + i] = ei_get_sign(buffer[ix + i]) * MAX_ACCEPTED_RANGE;
            }
        }

        buffer[ix + 0] *= CONVERT_G_TO_MS2;
        buffer[ix + 1] *= CONVERT_G_TO_MS2;
        buffer[ix + 2] *= CONVERT_G_TO_MS2;

        delayMicroseconds(next_tick - micros());
    }

    // Turn the raw buffer in a signal which we can the classify
    signal_t signal;
    int err = numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    if (err != 0) {
        Serial.println("Failed to create signal from buffer\n");
        return;
    }

    // Run the classifier
    ei_impulse_result_t result = { 0 };

    err = run_classifier(&signal, &result, debug_nn);
    if (err != EI_IMPULSE_OK) {
        Serial.println("ERR: Failed to run classifier\n");
        return;
    }

    // print the predictions
    Serial.print("Predictions ");
    Serial.print("(DSP: )");
    Serial.print(result.timing.dsp);
    Serial.print(" ms., Classification: ");
    Serial.print(result.timing.classification);
    Serial.print(" ms., Anomaly: ");
    Serial.print(result.timing.anomaly);
    Serial.print(" ms.)");
    Serial.println(": ");
    float bestval = 0.0;
    char bestlabel = 'c';
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        Serial.print("    ");
        Serial.print(result.classification[ix].label);
        Serial.print(": ");
        Serial.println(result.classification[ix].value);        
        if (result.classification[ix].value > bestval){
          bestval = result.classification[ix].value;
          bestlabel = result.classification[ix].label[0];
        }
    }
    value = bestlabel;
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    Serial.print("    anomaly score: ");
    Serial.print(result.anomaly);
#endif
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_ACCELEROMETER
#error "Invalid model for current sensor"
#endif