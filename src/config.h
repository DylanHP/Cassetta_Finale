/*
    Config Library

    Dylan Zanaglio
    Valsir development
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>
#include <movingAvg.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <AsyncTCP.h>
#include <Stepper.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <FS.h>
#include <SPIFFS.h>

// costants
// scheda wroom
const int LED_PIN = 18;
// const int LED_PIN = 12;
// const int SDA_PIN = 14;
// const int SCL_PIN = 13;

unsigned long AP_TIME = 10;  // misurato in minuti
const int STEPS_PER_REVOLUTION = 333;
const int MOTOR_PIN = 27;
unsigned int DISTANZA_MINIMA = 60;
unsigned int SECONDI_ATTIVAZIONE_AP = 15;
unsigned int SECONDI_ATTIVAZIONE_MOTOR = 10;

// values for program
unsigned int distanza;
unsigned int secondi;
unsigned long stableTimeMotor = 0;
unsigned long stableTimeAP = 0;
unsigned int avg;

// timers
unsigned long startTime;  // for ap timer
unsigned long lastSentTime = 0;
unsigned long previousMillis = 0;  // LED blink without delay

// steps for each revolution of drain
boolean active = false;
// per board wroom
Stepper myStepper(STEPS_PER_REVOLUTION, 25, 26, 32, 33);
// Stepper myStepper(STEPS_PER_REVOLUTION, 31, 28, 30, 33);

// for Ap timer
boolean apOn = true;

// motor driver
enum MotorState { FORWARD,
                  WAIT,
                  BACKWARD };
MotorState motorState = BACKWARD;
unsigned long motorTimer = 0;

// Istance of the sensor
VL53L0X sensor;

// Istance of moving average
movingAvg avgMesure(10);

#endif  // CONFIG_H