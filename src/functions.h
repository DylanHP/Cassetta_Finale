/*
  Functions library

  Dylan Zanaglio
  Valsir development

  This header file contains various utility functions for WiFi initialization,
  sensor measurement, access point management, LED blinking, and motor actuation.

  Functions:
  - void initWifi():
    Initializes the WiFi in Access Point mode and prints the AP IP address.

  - void doMeasure():
    Reads the sensor measurement and updates the average measurement.

  - void checkAP():
    Checks if the access point should be disabled based on the elapsed time.

  - void blinkWithoutDelay(int pin, int interval):
    Blinks an LED without using delay, allowing other code to run simultaneously.

  - void blinkFive():
    Blinks an LED five times with a 250ms interval.

  - void actuate_motor(int avg):
    Actuates a motor based on the average sensor measurement. It controls the motor
    state (FORWARD, WAIT, BACKWARD) and handles LED blinking and system restart
    based on certain conditions.
*/

#ifndef FUNCTIONS
#define FUNCTIONS

#include <config.h>

void initWifi() {
  WiFi.softAP(ssid, password);
  WiFi.mode(WIFI_AP);
  Serial.println("Access Point Enabled");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void doMeasure() {
  int measure = sensor.readRangeContinuousMillimeters();
  avg = avgMesure.reading(measure);
}

void checkAP() {
  if (apOn && millis() - startTime >= AP_TIME * 60 * 1000) {
    WiFi.softAPdisconnect(true);
    Serial.println("Access Point Disabled");
    apOn = false;
  }
}

void blinkWithoutDelay(int pin, int interval) {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time the LED was toggled
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa
    int ledState = digitalRead(pin);
    digitalWrite(pin, !ledState);
  }
}

void blinkFive() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
  }
}

void actuate_motor(int avg) {
  unsigned long currentMillis = millis();

  if (avg >= DISTANZA_MINIMA && avg <= distanza * 10) {
    digitalWrite(LED_PIN, HIGH);
    if (currentMillis - stableTimeMotor >= secondi * 1000) {
      Serial.print("la misura è stata corretta per più di: ");
      Serial.print(secondi);
      Serial.print(" sec\n");
      active = true;
      stableTimeMotor = currentMillis;  // Update the stable time after the
                                        // action has been performed
    }
  } else if (avg > distanza && active) {
    // activate motor
    digitalWrite(LED_PIN, LOW);
    digitalWrite(MOTOR_PIN, HIGH);

    switch (motorState) {
      case FORWARD:
        myStepper.step(STEPS_PER_REVOLUTION);
        motorState = BACKWARD;
        Serial.println("FORWARD");
        motorTimer = currentMillis;
        active = false;
        break;
      case WAIT:
        if (currentMillis - motorTimer >= SECONDI_ATTIVAZIONE_MOTOR * 1000) {
          motorState = FORWARD;
          Serial.println("WAIT");
        }
        break;
      case BACKWARD:
        myStepper.step(-STEPS_PER_REVOLUTION);
        motorState = WAIT;
        Serial.println("BACKWARD");
        break;
    }
  } else if (avg <= DISTANZA_MINIMA) {
    blinkWithoutDelay(LED_PIN, 1000);
    if (currentMillis - stableTimeAP >= secondi * 1000) {
      Serial.print("la misura è stata corretta per più di: ");
      Serial.print(secondi);
      Serial.print(" sec\n");
      digitalWrite(LED_PIN, LOW);
      // when is enabled
      stableTimeAP = currentMillis;
      ESP.restart();
    }
  } else {
    digitalWrite(MOTOR_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    stableTimeMotor = currentMillis;
    stableTimeAP = currentMillis;
  }
}

#endif