#include <Arduino.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <AsyncTCP.h>
#include <ElegantOTA.h>
#include "SPIFFS.h"

#include "configuration/Configuration.h"
#include "../speedCalculators/CalculatorTypes.h"
#include "sensors.h"
#include "calculators.h"

AsyncWebServer server(80);
DNSServer dns;
AsyncWiFiManager wifiManager(&server,&dns);
Configuration * theConfig;
std::vector<std::string> messages;
SensorApi sensorApi;
CalculatorApi calculatorApi;

void saveSettingsRequestHandler(AsyncWebServerRequest * request) {
  Serial.println("Saving settings");
  theConfig->save();
  messages.push_back("Configuration saved");
  messages.push_back("Please reset OpenAir for the new configuration to take effect.");
  request->redirect("/");
}

void startInterface(DI * container, Configuration *config) {
  theConfig = config;
  wifiManager.autoConnect(HOSTNAME, AP_PASSWORD);
  Serial.println("Wifi connected");

  sensorApi.initialize(container, &server, config);
  calculatorApi.initialize(container, &server, config);

  server.on("/config/save", HTTP_POST, saveSettingsRequestHandler);

  server.serveStatic("/static", SPIFFS, "/");

  server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html", false);
  });

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