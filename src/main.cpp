/*
  Main Program

  Dylan Zanaglio
  Valsir development

  This program initializes and runs a web server on an Arduino-based platform.
  It sets up various components including WiFi, WebSocket, routes, and a captive portal.
  It also initializes a sensor and a stepper motor, and handles sensor readings and motor actuation in the main loop.

  Libraries:
  - config.h: Configuration settings
  - web_server.h: Web server functionalities
  - functions.h: Additional functions
  - Arduino.h: Core Arduino functions

  Setup Function:
  - Serial.begin(115200): Initializes serial communication at 115200 baud rate.
  - SPIFFS.begin(true): Initializes the SPIFFS file system.
  - preferences.begin("my-app", false): Initializes preferences for saving data.
  - Wire.begin(): Initializes I2C communication.
  - avgMesure.begin(): Initializes the averaging measurement.
  - initWifi(): Initializes WiFi.
  - initWebSocket(): Initializes WebSocket.
  - initRoutes(): Initializes web server routes.
  - initCaptivePortal(): Initializes the captive portal.
  - sensor.init(): Initializes the sensor.
  - server.begin(): Starts the web server.
  - myStepper.setSpeed(45): Sets the speed of the stepper motor.
  - sensor.setTimeout(500): Sets the timeout for the laser sensor.
  - pinMode(LED_PIN, OUTPUT): Sets the LED pin as output.
  - pinMode(MOTOR_PIN, OUTPUT): Sets the motor pin as output.
  - sensor.startContinuous(): Starts continuous sensor readings.
  - blinkFive(): Blinks the LED five times.
  - distanza = preferences.getUInt("distanza", 5): Retrieves the previous distance value.
  - secondi = preferences.getUInt("secondi", 5): Retrieves the previous seconds value.

  Loop Function:
  - doMeasure(): Reads and averages the sensor data.
  - notifyClients(): Sends data to web clients every 500 milliseconds.
  - actuate_motor(avg): Actuates the motor based on the averaged sensor data.
  - checkAP(): Disables the access point after a certain time.
  - dnsHandle(): Handles DNS requests.
  - ws.cleanupClients(): Cleans up WebSocket clients.
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