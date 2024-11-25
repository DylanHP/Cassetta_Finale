/*
  Libreria di Configurazione

  Questo file header contiene le impostazioni di configurazione e le dichiarazioni per il progetto.

  Author: Dylan Zanaglio
  Organization: Valsir development

  Costanti:
  - LED_PIN: Pin del LED.
  - AP_TIME: Tempo di attivazione dell'Access Point (in minuti).
  - STEPS_PER_REVOLUTION: Numero di passi per rivoluzione del motore stepper.
  - MOTOR_PIN: Pin del motore.
  - DISTANZA_MINIMA: Distanza minima per l'attivazione.
  - SECONDI_ATTIVAZIONE_AP: Secondi di attivazione dell'Access Point.
  - SECONDI_ATTIVAZIONE_MOTOR: Secondi di attivazione del motore.

  Variabili:
  - distanza: Distanza misurata.
  - secondi: Secondi trascorsi.
  - stableTimeMotor: Tempo stabile per il motore.
  - stableTimeAP: Tempo stabile per l'Access Point.
  - avg: Media delle misurazioni.

  Timer:
  - startTime: Tempo di inizio per il timer dell'Access Point.
  - lastSentTime: Ultimo tempo di invio.
  - previousMillis: Tempo precedente per il lampeggio del LED senza delay.

  Motore stepper:
  - active: Stato di attivazione del motore.
  - myStepper: Istanza del motore stepper.
  - MotorState: Stati possibili del motore (FORWARD, WAIT, BACKWARD).
  - motorState: Stato iniziale del motore (BACKWARD).
  - motorTimer: Timer di azionamento del motore.

  Altre variabili:
  - apOn: Stato di attivazione dell'Access Point.
  - sensor: Istanza del sensore VL53L0X.
  - avgMesure: Istanza della media mobile per le misurazioni.
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

// Costanti
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

// Variabili
unsigned int distanza;
unsigned int secondi;
unsigned long stableTimeMotor = 0;
unsigned long stableTimeAP = 0;
unsigned int avg;

// Timers
unsigned long startTime;  // per timer ap
unsigned long lastSentTime = 0;
unsigned long previousMillis = 0;  // Lampeggio led senza delay

// Motore stepper
boolean active = false;                                   // per gli step del motore
Stepper myStepper(STEPS_PER_REVOLUTION, 25, 26, 32, 33);  // iniziallizzazione del motore
// Stepper myStepper(STEPS_PER_REVOLUTION, 31, 28, 30, 33);
enum MotorState { FORWARD,
                  WAIT,
                  BACKWARD };      // stati possibili
MotorState motorState = BACKWARD;  // stato iniziale
unsigned long motorTimer = 0;      // timer di azionamento del motore

boolean apOn = true;  // for Ap timer

VL53L0X sensor;  // Istance of the sensor

movingAvg avgMesure(10);  // Istanza della media mobile

#endif  // CONFIG_H