# Cassetta_finale

<div style="display: flex; justify-content: space-between;">
  <img src="/images/image.png" alt="Image 1" style="width: 30%; border-radius: 10px; margin-right: 10px;">
  <img src="/images/image-1.png" alt="Image 2" style="width: 30%; border-radius: 10px; margin-right: 10px;">
  <img src="/images/image-2.png" alt="Image 3" style="width: 30%; border-radius: 10px;">
</div>

## Descrizione

Il progetto utilizza un microcontrollore ESP32 per gestire un server web asincrono, un motore passo-passo, e un sensore di distanza VL53L0X. Il sistema è progettato per misurare distanze, attivare un motore in base alle misurazioni e fornire un'interfaccia web per monitorare e controllare il sistema.

## Componenti

- **ESP32**: Microcontrollore utilizzato per eseguire il codice.
- **VL53L0X**: Sensore di distanza utilizzato per misurare le distanze.
- **Motore passo-passo**: Utilizzato per attuare movimenti basati sulle misurazioni del sensore.
- **LED**: Utilizzato per indicazioni visive.
- **SPIFFS**: File system utilizzato per memorizzare i file del server web.
- **WiFi**: Utilizzato per la connettività di rete.

## Librerie Utilizzate

- `WiFi.h`: Per la connettività WiFi.
- `movingAvg.h`: Per il calcolo delle medie mobili.
- `Wire.h`: Per la comunicazione I2C.
- `VL53L0X.h`: Per l'interfacciamento con il sensore VL53L0X.
- `AsyncTCP.h`: Per la comunicazione TCP asincrona.
- `Stepper.h`: Per il controllo dei motori passo-passo.
- `Arduino.h`: Libreria core di Arduino.
- `ArduinoJson.h`: Per il parsing e la serializzazione JSON.
- `DNSServer.h`: Per la funzionalità del server DNS.
- `ESPAsyncWebServer.h`: Per la funzionalità del server web asincrono.
- `Preferences.h`: Per memorizzare le preferenze in memoria non volatile.
- `FS.h`: Per le operazioni del file system.
- `SPIFFS.h`: Per le operazioni del file system SPI Flash.

## Configurazione

### Costanti

- `LED_PIN`: Pin GPIO per il LED.
- `AP_TIME`: Tempo di accesso in modalità Access Point (minuti).
- `STEPS_PER_REVOLUTION`: Numero di passi per rivoluzione del motore passo-passo.
- `MOTOR_PIN`: Pin GPIO per il motore.
- `DISTANZA_MINIMA`: Soglia minima di distanza.
- `SECONDI_ATTIVAZIONE_AP`: Tempo di attivazione per l'access point (secondi).
- `SECONDI_ATTIVAZIONE_MOTOR`: Tempo di attivazione per il motore (secondi).

### Variabili

- `distanza`: Misurazione della distanza.
- `secondi`: Tempo in secondi.
- `stableTimeMotor`: Tempo stabile per l'operazione del motore.
- `stableTimeAP`: Tempo stabile per l'operazione dell'access point.
- `avg`: Valore medio.

### Timer

- `startTime`: Tempo di inizio per il timer dell'access point.
- `lastSentTime`: Ultimo tempo di invio per la trasmissione dei dati.
- `previousMillis`: Millisecondi precedenti per il lampeggio del LED senza ritardo.

### Motore Passo-Passo

- `active`: Flag booleano che indica se il motore è attivo.
- `myStepper`: Istanza del motore passo-passo per la scheda WROOM.

### Access Point

- `apOn`: Flag booleano che indica se l'access point è attivo.

### Driver del Motore

- `MotorState`: Enumerazione per gli stati del motore (FORWARD, WAIT, BACKWARD).
- `motorState`: Stato corrente del motore.
- `motorTimer`: Timer per l'operazione del motore.

### Sensore

- `sensor`: Istanza del sensore VL53L0X.

### Media Mobile

- `avgMesure`: Istanza della classe di media mobile con una finestra di 10.

## Funzioni

### Funzioni Principali

- `void initWifi()`: Inizializza il WiFi in modalità Access Point e stampa l'indirizzo IP dell'AP.
- `void doMeasure()`: Legge la misurazione del sensore e aggiorna la media delle misurazioni.
- `void checkAP()`: Controlla se l'access point deve essere disabilitato in base al tempo trascorso.
- `void blinkWithoutDelay(int pin, int interval)`: Lampeggia un LED senza usare `delay()`, permettendo l'esecuzione simultanea di altro codice.
- `void blinkFive()`: Lampeggia un LED cinque volte con un intervallo di 250ms.
- `void actuate_motor(int avg)`: Attiva un motore basato sulla media delle misurazioni del sensore. Controlla lo stato del motore (FORWARD, WAIT, BACKWARD) e gestisce il lampeggio del LED e il riavvio del sistema in base a determinate condizioni.

### Funzioni del Server Web

- `void initWebSocket()`: Inizializza il server WebSocket e imposta i gestori degli eventi.
- `void initRoutes()`: Imposta le rotte per il server web.
- `void initCaptivePortal()`: Inizializza il captive portal per le connessioni degli utenti.
- `void notifyClients()`: Invia dati aggiornati a tutti i client WebSocket connessi.
- `void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)`: Gestisce i messaggi WebSocket in arrivo.
- `void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)`: Gestisce gli eventi WebSocket.
- `void dnsHandle()`: Gestisce le richieste DNS e le informazioni di connessione degli utenti.

## Installazione

1. Clona il repository.
2. Apri il progetto con il tuo IDE preferito (ad esempio, PlatformIO o Arduino IDE).
3. Carica il codice sulla tua scheda ESP32.
4. Collega il sensore VL53L0X e il motore passo-passo ai pin corretti.
5. Configura le credenziali WiFi nel file `config.h`.
6. Avvia il dispositivo e connettiti all'access point creato dall'ESP32.

## Utilizzo

1. Connettiti all'access point creato dall'ESP32.
2. Apri un browser web e naviga all'indirizzo IP dell'ESP32.
3. Utilizza l'interfaccia web per monitorare le misurazioni del sensore e controllare il motore.

## Autore

Dylan Zanaglio - Valsir development
