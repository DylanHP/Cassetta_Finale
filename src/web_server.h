/*
    Web Server Library

    Dylan Zanaglio
    Valsir development
*/

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <config.h>

// Create AsyncWebServer object on port 80 and a dns server
DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//  Ap network credencials
const char *ssid = "Valsir_Placca";
const char *password = "12345678";

// inizialization of the preference to save the value of distanza and secondi
Preferences preferences;

// Variables for user connection
String user_name;
String proficiency;
bool name_received = false;
bool proficiency_received = false;

// Functions definition for usage in main
void initWebSocket();
void initRoutes();
void initCaptivePortal();
void notifyClients();

// Functions
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

void initCaptivePortal() {
  // adding handler for captive portal
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler())
      .setFilter(ON_AP_FILTER);  // only when requested from AP
}

void initRoutes() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
}

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

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    data[len] = 0;
    // Parse the JSON data
    JsonDocument doc;
    deserializeJson(doc, (char *)data);

    // putting values in the memory namepace
    distanza = doc["distanza"];
    secondi = doc["secondi"];
    preferences.putUInt("distanza", distanza);
    preferences.putUInt("secondi", secondi);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                    client->remoteIP().toString().c_str());
      // notify the new values
      notifyClients();
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void dnsHandle() {
  // Dns handle
  dnsServer.processNextRequest();
  if (name_received && proficiency_received) {
    Serial.print("Hello ");
    Serial.println(user_name);
    Serial.print("You have stated your proficiency to be ");
    Serial.println(proficiency);
    name_received = false;
    proficiency_received = false;
    Serial.println("We'll wait for the next client now");
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

#endif