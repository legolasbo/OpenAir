#include <Arduino.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <AsyncTCP.h>
#include <ElegantOTA.h>
#include "SPIFFS.h"

#include "constants.h"

AsyncWebServer server(80);
DNSServer dns;

String processor(const String& var) {
  if (var == "STATE") {
    return "JEEJ!";
  }
  return "OOPS!?";
}

void startInterface() {
    if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS MOUNT FAILED!");
  }

  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.autoConnect(HOSTNAME, AP_PASSWORD);

  Serial.println("Wifi connected");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html", false, processor);
  });
  server.serveStatic("/", SPIFFS, "/");

  ElegantOTA.begin(&server);
  server.begin();
}

ulong lastCheck = 0;
void checkWiFi() {
  ulong now = millis();
  if (now < lastCheck) {
    lastCheck = now;
    return;
  }

  if (now - lastCheck > 5000) {
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to WiFi....");
    WiFi.disconnect();
    WiFi.reconnect();
    lastCheck = now;
  }
}

void loopInterface() {
  ElegantOTA.loop();
  checkWiFi();
}