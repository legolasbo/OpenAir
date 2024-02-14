#include <Arduino.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <AsyncTCP.h>
#include <ElegantOTA.h>
#include "SPIFFS.h"

#include "configuration/Configuration.h"
#include "../speedCalculators/CalculatorTypes.h"
#include "constants.h"

AsyncWebServer server(80);
DNSServer dns;
Configuration * theConfig;

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

void appendAddSensorFormTo(std::ostringstream &out, ConnectionType conn, SensorType sens) {
  out << "<form action=\"/config/sensors/add\" method=\"post\">";
  out << ToString(sens);
  out << "<input type=\"hidden\" name=\"connectionType\" value=\"" << conn << "\" />";
  out << "<input type=\"hidden\" name=\"sensorType\" value=\"" << sens << "\" />";
  appendRadioButton(out, "connection", ToMachineName(X4), String(X4).c_str(), ToString(X4));
  appendRadioButton(out, "connection", ToMachineName(X6), String(X6).c_str(), ToString(X6));
  out << "<input type=\"submit\" value=\"Add\">";
  out << "</form>";
}

void appendSensorListItemTo(std::ostringstream &out, ConnectionType conn, SensorType sens) {
  out << "<li>";
  appendAddSensorFormTo(out, conn, sens);
  out << "</li>";
}

String createSaveSettingsFormTo() {
  std::ostringstream out;
  out << "<form action=\"/config/save\" method=\"post\">";
  out << "<input type=\"submit\" value=\"Save settings\">";
  out << "</form>";
  return out.str().c_str();
}

String createConfigurableSensorList() {
  std::ostringstream out;
  out << "<h3>Physical sensors</h3>";
  out << "<ul>";
  appendSensorListItemTo(out, I2C, ThreePositionSwitchSensor);
  appendSensorListItemTo(out, I2C, SHT20Sensor);
  out << "</ul>";

  out << "<h3>Virtual sensors</h3>";
  out << "<ul>";
  out << "</ul>";

  return out.str().c_str();
}

void appendSensorListTableTo(std::ostringstream &out, std::vector<SensorConfiguration> sensors) {
  out << "<table>\n";
  out << "<thead><tr>";
  out << "<th>sensor type</th>";
  out << "<th>connection type</th>";
  out << "</tr></thead><tbody>";
  for (SensorConfiguration c : sensors) {
    out << "<tr>";
    out << "<td>" << ToString(c.getSensorType()) << "</td>";
    out << "<td>" << ToString(c.getConnectionType()) << "</td>";
    out << "</tr>";
  }
  out << "</tbody></table>\n";
}

void appendSensorListTableForConnector(std::ostringstream &out, SensorConnector connector) {
  out << "<h2>Connector: " << ToString(connector) << "</h2>";
  std::vector<SensorConfiguration> sensors = theConfig->getSensors()->getConfigurationsFor(connector);
  appendSensorListTableTo(out, sensors);
}

String createConfiguredSensorsHTML() {
  std::ostringstream out;

  appendSensorListTableForConnector(out, X4);
  appendSensorListTableForConnector(out, X6);

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

  std::vector<CalculatorConfiguration *> calculators = theConfig->getCalculators()->all();

  for (size_t i = 0; i < calculators.size(); i++) {
    out << calculators.at(i)->editForm();
  }

  return out.str().c_str();
}

String processor(const String& var) {
  if (var == "CONFIGURABLE_SENSOR_LIST") {
    return createConfigurableSensorList();
  }

  if (var == "CONFIGURED_SENSORS") {
    return createConfiguredSensorsHTML();
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

void internalServerErrorResponse(AsyncWebServerRequest * request, String text) {
    Serial.print("ERROR: ");
    Serial.println(text);
    request->send(500, "text/html", text);
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

  SensorConfiguration config(connector, connType, sensType);

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
      Serial.println(theConfig->getCalculators()->all().size());
      request->redirect("/config/calculators");
      break;
    default:
      return internalServerErrorResponse(request, "unknown calculator type");
  }
}

void saveSettingsRequestHandler(AsyncWebServerRequest * request) {
  Serial.println("Saving settings");
  theConfig->save();
}

void startInterface(Configuration *config) {
  theConfig = config;

  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.autoConnect(HOSTNAME, AP_PASSWORD);

  Serial.println("Wifi connected");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html", false, processor);
  });

  server.on("/config/sensors", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/sensors.html", "text/html", false, processor);
  });
  server.on("/config/sensors/add", HTTP_POST, addSensorRequestHandler);

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