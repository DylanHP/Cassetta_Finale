/*
    Main Program

    Dylan Zanaglio
    Valsir development
*/
#include <config.h>
#include <web_server.h>
#include <functions.h>
#include "Arduino.h"

/****************************Setup********************************/
void setup() {
  // begins
  Serial.begin(115200);
  if (!SPIFFS.begin(true)) return;
  preferences.begin("my-app",
                    false);  // saving data from web

  // scheda wroom
  Wire.begin();
  // pinMode(SDA_PIN, INPUT_PULLUP);
  // pinMode(SCL_PIN, INPUT_PULLUP);
  // Wire.begin(SDA_PIN, SCL_PIN);
  avgMesure.begin();

  // inits
  initWifi();
  initWebSocket();
  initRoutes();
  initCaptivePortal();
  if (!sensor.init()) return;

  // inizializzo prima tutto e poi avvio
  server.begin();

  // settings
  myStepper.setSpeed(45);      // setting the speed of the stepper
  sensor.setTimeout(500);      // Timeout setup for laser
  pinMode(LED_PIN, OUTPUT);    // LED as output
  pinMode(MOTOR_PIN, OUTPUT);  // MotorPin as output

  // Startup of sensor readings
  sensor.startContinuous();
  blinkFive();

  // Recupera i vecchi valori prima del reset
  distanza = preferences.getUInt("distanza", 5);
  secondi = preferences.getUInt("secondi", 5);
}

/****************************Loop********************************/
void loop() {
  doMeasure();  // reading and averaging the sensor

  unsigned long currentTime = millis();
  if (currentTime - lastSentTime >= 500) {
    notifyClients();  // sending data to web
  }

  actuate_motor(avg);  // actuate the motor

  checkAP();  // disable ap after AP_TIME

  dnsHandle();  // DNS handle

  ws.cleanupClients();  // cleanups
}