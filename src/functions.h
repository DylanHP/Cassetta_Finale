/*
  Libreria di funzioni

  Author: Dylan Zanaglio
  Organization: Valsir development

  Questo file header contiene varie funzioni di utilità per l'inizializzazione del WiFi,
  la misurazione dei sensori, la gestione dell'access point, il lampeggio dei LED e l'azionamento del motore.

  Funzioni:
  - void initWifi():
    Inizializza il WiFi in modalità Access Point e stampa l'indirizzo IP dell'AP.

  - void doMeasure():
    Legge la misurazione del sensore e aggiorna la misurazione media.

  - void checkAP():
    Controlla se l'access point deve essere disabilitato in base al tempo trascorso.

  - void blinkWithoutDelay(int pin, int interval):
    Lampeggia un LED senza usare delay, permettendo l'esecuzione simultanea di altro codice.

  - void blinkFive():
    Lampeggia un LED cinque volte con un intervallo di 250ms.

  - void actuate_motor(int avg):
    Attiva un motore in base alla misurazione media del sensore. Controlla lo stato del motore
    (AVANTI, ATTESA, INDIETRO) e gestisce il lampeggio dei LED e il riavvio del sistema
    in base a determinate condizioni.
*/

#ifndef FUNCTIONS
#define FUNCTIONS

#include <config.h>

// Iniziallizazione del WiFi
void initWifi() {
  WiFi.softAP(ssid, password);
  WiFi.mode(WIFI_AP);
  Serial.println("Access Point Enabled");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

// Esegue la misurazione del sensore
void doMeasure() {
  int measure = sensor.readRangeContinuousMillimeters();
  avg = avgMesure.reading(measure);
}

// Controlla se l'access point deve essere disabilitato
void checkAP() {
  if (apOn && millis() - startTime >= AP_TIME * 60 * 1000) {
    WiFi.softAPdisconnect(true);
    Serial.println("Access Point Disabled");
    apOn = false;
  }
}

// Lampeggia un LED senza usare delay
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

// Lampeggia un LED cinque volte
void blinkFive() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
  }
}

// Attiva il motore in base alla misura del sensore
void actuate_motor(int avg) {
  unsigned long currentMillis = millis();

  if (avg >= DISTANZA_MINIMA && avg <= distanza * 10) {
    digitalWrite(LED_PIN, HIGH);
    if (currentMillis - stableTimeMotor >= secondi * 1000) {
      Serial.print("la misura è stata corretta per più di: ");
      Serial.print(secondi);
      Serial.print(" sec\n");
      active = true;
      stableTimeMotor = currentMillis;
    }
  } else if (avg > distanza && active) {
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