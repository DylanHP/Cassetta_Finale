
/*
  Libreria per il Server Web

  Autore: Dylan Zanaglio
  Organizzazione: Valsir development

  Descrizione:
  Questa libreria gestisce un server web asincrono, un server DNS e un server WebSocket
  per un access point WiFi. Include la gestione di un captive portal e la comunicazione
  tramite WebSocket per aggiornare i client con i dati attuali.

  Variabili:
  - DNSServer dnsServer: Istanza del server DNS.
  - AsyncWebServer server: Istanza del server web asincrono sulla porta 80.
  - AsyncWebSocket ws: Istanza del server WebSocket con endpoint "/ws".
  - const char *ssid: Nome dell'access point WiFi.
  - const char *password: Password dell'access point WiFi.
  - Preferences preferences: Istanza per la gestione delle preferenze.
  - String user_name: Nome dell'utente ricevuto tramite WebSocket.
  - String proficiency: Competenza dell'utente ricevuta tramite WebSocket.
  - bool name_received: Flag per indicare se il nome è stato ricevuto.
  - bool proficiency_received: Flag per indicare se la competenza è stata ricevuta.

  Funzioni:
  - void initWebSocket(): Inizializza il server WebSocket.
  - void initRoutes(): Inizializza le rotte del server web.
  - void initCaptivePortal(): Inizializza il captive portal.
  - void notifyClients(): Notifica i client con i dati attuali.
  - void handleWebSocketMessage(void *arg, uint8_t *data, size_t len): Gestisce i messaggi ricevuti tramite WebSocket.
  - void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len): Callback per gli eventi WebSocket.
  - void dnsHandle(): Gestisce le richieste DNS.

  Classe:
  - CaptiveRequestHandler: Gestisce le richieste per il captive portal.
    - bool canHandle(AsyncWebServerRequest *request): Verifica se la richiesta può essere gestita.
    - void handleRequest(AsyncWebServerRequest *request): Gestisce la richiesta inviando la pagina index.html.
*/

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <config.h>

DNSServer dnsServer;        // Crea un istanza del server dns
AsyncWebServer server(80);  // Crea un istanza del server web
AsyncWebSocket ws("/ws");   // Crea un istanza del server websocket

const char *ssid = "Valsir_Placca";  // Nome dell'access point
const char *password = "12345678";   // Password dell'access point

Preferences preferences;  // Inizializzo le preferenze

// Variabili per la connessione websocket
String user_name;
String proficiency;
bool name_received = false;
bool proficiency_received = false;

// Definizione delle funzioni
void initWebSocket();
void initRoutes();
void initCaptivePortal();
void notifyClients();

// Handler per il captive portal
class CaptiveRequestHandler : public AsyncWebHandler {
 public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request) {
    // request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  }
};

// Inizializza il captive portal
void initCaptivePortal() {
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler())
      .setFilter(ON_AP_FILTER);  // solo quando richiesta da un client connesso all'AP
}

// Inizializza le rotte del server web
void initRoutes() {
  // Rotta per la pagina principale
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
}

// Notifica i client con i dati attuali per aggiornare le letture e i dati salvati
void notifyClients() {
  JsonDocument doc;
  doc["distanza"] = distanza;
  doc["secondi"] = secondi;
  doc["liveMeasurement"] = avg / 10;
  String output;
  // Serial.println(output);
  serializeJson(doc, output);
  ws.textAll(output);
}

// Gestisce i messaggi inviati tramite websocket
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    data[len] = 0;
    // Parsing del messaggio JSON
    JsonDocument doc;
    deserializeJson(doc, (char *)data);

    // Inserimento dei dati ricevuti nelle variabili
    distanza = doc["distanza"];
    secondi = doc["secondi"];
    preferences.putUInt("distanza", distanza);
    preferences.putUInt("secondi", secondi);
  }
}

// Funzione di callback per il websocket
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("Client WebSocket #%u connesso da %s\n", client->id(),
                    client->remoteIP().toString().c_str());
      // Notifica il client appena connesso con i dati attuali
      notifyClients();
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("Client WebSocket #%u disconnesso\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

// Gestisce le richieste DNS
void dnsHandle() {
  dnsServer.processNextRequest();
  if (name_received && proficiency_received) {
    Serial.print("Ciao ");
    Serial.println(user_name);
    Serial.print("Hai dichiarato che la tua competenza è ");
    Serial.println(proficiency);
    name_received = false;
    proficiency_received = false;
    Serial.println("Aspetteremo ora il prossimo cliente");
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

#endif