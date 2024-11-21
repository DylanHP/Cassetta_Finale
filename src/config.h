/*
    Config Library

    This header file contains configuration settings and declarations for the project.

    Author: Dylan Zanaglio
    Organization: Valsir development

    Libraries Included:
    - WiFi.h: For WiFi connectivity.
    - movingAvg.h: For calculating moving averages.
    - Wire.h: For I2C communication.
    - VL53L0X.h: For interfacing with the VL53L0X sensor.
    - AsyncTCP.h: For asynchronous TCP communication.
    - Stepper.h: For controlling stepper motors.
    - Arduino.h: Core Arduino library.
    - ArduinoJson.h: For JSON parsing and serialization.
    - DNSServer.h: For DNS server functionality.
    - ESPAsyncWebServer.h: For asynchronous web server functionality.
    - Preferences.h: For storing preferences in non-volatile storage.
    - FS.h: For file system operations.
    - SPIFFS.h: For SPI Flash File System operations.

    Constants:
    - LED_PIN: GPIO pin for the LED.
    - AP_TIME: Access point time in minutes.
    - STEPS_PER_REVOLUTION: Number of steps per revolution for the stepper motor.
    - MOTOR_PIN: GPIO pin for the motor.
    - DISTANZA_MINIMA: Minimum distance threshold.
    - SECONDI_ATTIVAZIONE_AP: Activation time for access point in seconds.
    - SECONDI_ATTIVAZIONE_MOTOR: Activation time for motor in seconds.

    Variables:
    - distanza: Distance measurement.
    - secondi: Time in seconds.
    - stableTimeMotor: Stable time for motor operation.
    - stableTimeAP: Stable time for access point operation.
    - avg: Average value.

    Timers:
    - startTime: Start time for access point timer.
    - lastSentTime: Last sent time for data transmission.
    - previousMillis: Previous milliseconds for LED blink without delay.

    Stepper Motor:
    - active: Boolean flag indicating if the motor is active.
    - myStepper: Stepper motor instance for the WROOM board.

    Access Point:
    - apOn: Boolean flag indicating if the access point is on.

    Motor Driver:
    - MotorState: Enumeration for motor states (FORWARD, WAIT, BACKWARD).
    - motorState: Current state of the motor.
    - motorTimer: Timer for motor operation.

    Sensor:
    - sensor: Instance of the VL53L0X sensor.

    Moving Average:
    - avgMesure: Instance of the moving average class with a window size of 10.

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