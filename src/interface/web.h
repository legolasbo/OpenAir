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

AsyncWebServer server(80);
DNSServer dns;
AsyncWiFiManager wifiManager(&server,&dns);
Configuration * theConfig;
std::vector<std::string> messages;
SensorApi sensorApi;

void appendHtmlAttributeTo(std::ostringstream &out, const char * attributeName, const char * attributeValue) {
  out << " ";
  out << attributeName;
  out << "=\"";
  out << attributeValue;
  out << "\"";
}

void appendRadioButton(std::ostringstream &out, const char* elementName, const char* elementId, const char* elementValue, const char* label) {
  out << "<input";
  appendHtmlAttributeTo(out, "type", "radio");
  appendHtmlAttributeTo(out, "id", elementId);
  appendHtmlAttributeTo(out, "name", elementName);
  appendHtmlAttributeTo(out, "value", elementValue);
  out << ">\n";
  out << "<label";
  appendHtmlAttributeTo(out, "for", elementId);
  out  << ">" << label;
  out << "</label>";
}

String createSaveSettingsFormTo() {
  std::ostringstream out;
  out << "<form action=\"/config/save\" method=\"post\">";
  out << "<input type=\"submit\" value=\"Save settings\">";
  out << "</form>";
  return out.str().c_str();
}

void appendAddCalculatorForm(std::ostringstream &out, CalculatorType type) {
  out << "<form method=\"post\" action=\"/config/calculators/add\">";
  out << "<input type=\"hidden\" name=\"machineName\" value=\"" << ToMachineName(type) << "\" />";
  out << ToString(type);
  out << "<input type=\"submit\" value=\"Add\" />";
  out << "</form>";
}


String createConfigurableCalculatorList() {
  std::ostringstream out;

  out << "<ul>";

  out << "<li>";
  appendAddCalculatorForm(out, SHT20Calculator);
  out << "</li>";
  out << "<li>";
  appendAddCalculatorForm(out, ThreePositionSwitchCalculator);
  out << "</li>";
  
  out << "</ul>";

  return out.str().c_str();
}

String createConfiguredCalculatorsList() {
  std::ostringstream out;

  std::vector<std::string> uuids = theConfig->getCalculators()->getUuids();

  for (auto uuid : uuids) {
    out << theConfig->getCalculators()->get(uuid.c_str())->editForm();
  }

  return out.str().c_str();
}

String processor(const String& var) {
  if (var == "MESSAGES") {
    String out = "";
    for (std::string msg : messages) {
      out.concat(msg.c_str());
      out.concat("<br>");
    }
    messages.clear();
    return out;
  }

  if (var == "CONFIGURABLE_CALCULATOR_LIST") {
    return createConfigurableCalculatorList();
  }

  if (var == "CONFIGURED_CALCULATORS") {
    return createConfiguredCalculatorsList();
  }

  if (var == "SAVE_SETTINGS") {
    return createSaveSettingsFormTo();
  }

  return "OOPS!?";
}

void addSensorRequestHandler(AsyncWebServerRequest * request) {
  Serial.println("Add sensor handler");
  if (!request->hasArg("connectionType")) {
    return internalServerErrorResponse(request, "missing connectionType parameter");
  };
  if (!request->hasArg("sensorType")) {
    return internalServerErrorResponse(request, "missing sensorType parameter");
  }
  if (!request->hasArg("connection")) {
    return internalServerErrorResponse(request, "missing connection parameter");
  }

  SensorType sensType = SensorType(request->arg("sensorType").toInt());
  ConnectionType connType = ConnectionType(request->arg("connectionType").toInt());
  SensorConnector connector = SensorConnector(request->arg("connection").toInt());

  SensorConfiguration * config = new SensorConfiguration(connector, connType, sensType);

  if (theConfig->getSensors()->identicalConfigExists(config)) {
    return internalServerErrorResponse(request, "Unable to add this configuration. it is already present");
  }

  theConfig->getSensors()->add(config);
  request->redirect("/config/sensors");
}

void addCalculatorRequestHandler(AsyncWebServerRequest * request) {
  Serial.println("Add calculator handler");

  if (!request->hasArg("machineName")) {
    return internalServerErrorResponse(request, "Missing calculator machine name");
  }


  CalculatorType theType = CalculatorTypeFromMachineName(request->arg("machineName").c_str());
  switch(theType) {
    case ThreePositionSwitchCalculator:
    case SHT20Calculator:
      theConfig->getCalculators()->addNew(theType);
      request->redirect("/config/calculators");
      break;
    default:
      return internalServerErrorResponse(request, "unknown calculator type");
  }
}

void saveSettingsRequestHandler(AsyncWebServerRequest * request) {
  Serial.println("Saving settings");
  theConfig->save();
  messages.push_back("Configuration saved");
  messages.push_back("Please reset OpenAir for thenew configuration to take effect.");
  request->redirect("/");
}

void startInterface(Configuration *config) {
  theConfig = config;

  wifiManager.autoConnect(HOSTNAME, AP_PASSWORD);

  Serial.println("Wifi connected");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html", false, processor);
  });

  sensorApi.initialize(&server, config);

  server.on("/config/calculators", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/calculators.html", "text/html", false, processor);
  });
  server.on("/config/calculators/add", HTTP_POST, addCalculatorRequestHandler);

  server.on("/config/save", HTTP_POST, saveSettingsRequestHandler);

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