#include <Arduino.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <AsyncTCP.h>
#include <ElegantOTA.h>
#include "SPIFFS.h"

#include "sensors.h"
#include "calculators.h"
#include "mqtt.h"
#include "fileSystem.h"

class Web {
  private:
    AsyncWebServer server;
    DNSServer dns;
    AsyncWiFiManager wifiManager;

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

  public:
    Web() :
              server(80) , 
              wifiManager(&server, &dns) {
    }

    void begin() {
      wifiManager.autoConnect(HOSTNAME, AP_PASSWORD);
      Log.infoln("WiFi connected");

      this->server.serveStatic("/static", SPIFFS, "/");
      this->server.onNotFound([](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/index.html", "text/html", false);
      });

      DI::GetContainer()->resolve<SensorApi>()->configureCallbacks(server);
      DI::GetContainer()->resolve<CalculatorApi>()->configureCallbacks(server);
      DI::GetContainer()->resolve<MqttAPI>()->configureCallbacks(server);
      DI::GetContainer()->resolve<FileSystem>()->configureCallbacks(server);

      ElegantOTA.begin(&this->server);
      this->server.begin();
    }

    void loop() {
      ElegantOTA.loop();
      checkWiFi();
    }

};

