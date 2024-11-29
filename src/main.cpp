/*
  Programma principale

  Author: Dylan Zanaglio
  Organization: Valsir development

  Questo programma inizializza ed esegue un server web su una piattaforma basata su Arduino.
  Configura vari componenti tra cui WiFi, WebSocket, rotte e un captive portal.
  Inizializza anche un sensore e un motore passo-passo, e gestisce le letture del sensore e l'attuazione del motore nel loop principale.

  Librerie:
  - config.h: Impostazioni di configurazione
  - web_server.h: Funzionalità del server web
  - functions.h: Funzioni aggiuntive
  - Arduino.h: Funzioni core di Arduino

  Funzione Setup:
  - Serial.begin(115200): Inizializza la comunicazione seriale a 115200 baud.
  - SPIFFS.begin(true): Inizializza il file system SPIFFS.
  - preferences.begin("my-app", false): Inizializza le preferenze per salvare i dati.
  - Wire.begin(): Inizializza la comunicazione I2C.
  - avgMesure.begin(): Inizializza la misurazione media.
  - initWifi(): Inizializza il WiFi.
  - initWebSocket(): Inizializza il WebSocket.
  - initRoutes(): Inizializza le rotte del server web.
  - initCaptivePortal(): Inizializza il captive portal.
  - sensor.init(): Inizializza il sensore.
  - server.begin(): Avvia il server web.
  - myStepper.setSpeed(45): Imposta la velocità del motore passo-passo.
  - sensor.setTimeout(500): Imposta il timeout per il sensore laser.
  - pinMode(LED_PIN, OUTPUT): Imposta il pin del LED come uscita.
  - pinMode(MOTOR_PIN, OUTPUT): Imposta il pin del motore come uscita.
  - sensor.startContinuous(): Avvia le letture continue del sensore.
  - blinkFive(): Lampeggia il LED cinque volte.
  - distanza = preferences.getUInt("distanza", 5): Recupera il valore precedente della distanza.
  - secondi = preferences.getUInt("secondi", 5): Recupera il valore precedente dei secondi.

  Funzione Loop:
  - doMeasure(): Legge e media i dati del sensore.
  - notifyClients(): Invia dati ai client web ogni 500 millisecondi.
  - actuate_motor(avg): Attiva il motore in base ai dati mediati del sensore.
  - checkAP(): Disabilita l'access point dopo un certo tempo.
  - dnsHandle(): Gestisce le richieste DNS.
  - ws.cleanupClients(): Pulisce i client WebSocket.
*/

#include <config.h>
#include <web_server.h>
#include <functions.h>
#include "Arduino.h"

unsigned long previusTime = 0;
unsigned long interval = 500;

/****************************Setup********************************/
void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin(true)) return;
  preferences.begin("my-app", false);  // Salvo i dati delle preferenze
  Wire.begin();
  // pinMode(SDA_PIN, INPUT_PULLUP);
  // pinMode(SCL_PIN, INPUT_PULLUP);
  // Wire.begin(SDA_PIN, SCL_PIN);
  avgMesure.begin();

  // iniziallizzazioni
  initWifi();                  // inizializzo il wifi
  initWebSocket();             // inizializzo il websocket
  initRoutes();                // inizializzo le rotte
  initCaptivePortal();         // inizializzo il captive portal
  if (!sensor.init()) return;  // inizializzo il laser
  server.begin();              // inizializzo il server

  // impostazioni del motore
  myStepper.setSpeed(45);      // velocità del motore
  sensor.setTimeout(500);      // timeout del laser
  pinMode(LED_PIN, OUTPUT);    // ledPin come output
  pinMode(MOTOR_PIN, OUTPUT);  // MotorPin come output

  sensor.startContinuous();  // setto le letture del laser come continue
  blinkFive();               // lampeggio del led

  // Recupera i vecchi valori prima del reset
  distanza = preferences.getUInt("distanza", 5);
  secondi = preferences.getUInt("secondi", 5);
}

/****************************Loop********************************/
void loop() {
  unsigned long currentMills = millis();
  doMeasure();  // misura e media i dati letti dal laser

  // invia i dati ai client ogni 500 millisecondi
  unsigned long currentTime = millis();
  if (currentTime - lastSentTime >= 500) {
    notifyClients();
  }

  actuate_motor(avg);  // attiva il motore in base alla distanza letta
  checkAP();           // disabilita l'access point dopo un certo tempo
  if (currentMills - previusTime >= interval) {
    previousMillis = currentMills;
    dnsHandle();  // gestisce le richieste DNS
  }
  ws.cleanupClients();  // pulisce i client WebSocket
}