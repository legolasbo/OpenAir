

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
struct {
  SensorFactory *factory;
  SensorConfigurations *sensorConfigurations;
} webResources;

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
  out << "<thead><th>";
  out << "<td>sensor type</td>";
  out << "<td>connection type</td>";
  out << "</th></thead><tbody>";
  for (SensorConfiguration c : sensors) {
    out << "<tr>";
    out << "<td>" << ToString(c.getSensorType()) << "</td>";
    out << "<td>" << c.getConnectionType() << "</td>";
    out << "</tr>";
  }
  out << "</tbody></table>\n";
}

void appendSensorListTableForConnector(std::ostringstream &out, SensorConnector connector) {
  out << "<h2>Connector: " << ToString(connector) << "</h2>";
  std::vector<SensorConfiguration> sensors = webResources.sensorConfigurations->getConfigurationsFor(connector);
  appendSensorListTableTo(out, sensors);
}

String createConfiguredSensorsHTML() {
  std::ostringstream out;

  appendSensorListTableForConnector(out, X4);
  appendSensorListTableForConnector(out, X6);

  return out.str().c_str();
}

String processor(const String& var) {
  if (var == "CONFIGURABLE_SENSOR_LIST") {
    return createConfigurableSensorList();
  }

  if (var == "CONFIGURED_SENSORS") {
    return createConfiguredSensorsHTML();
  }

  return "OOPS!?";
}

void internalServerErrorResponse(AsyncWebServerRequest * request, String text) {
    Serial.print("ERROR: ");
    Serial.println(text);
    request->send(500, text);
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

  if (webResources.sensorConfigurations->identicalConfigExists(config)) {
    return internalServerErrorResponse(request, "Unable to add this configuration. it is already present");
  }

  webResources.sensorConfigurations->add(config);
  request->redirect("/config/sensors");
}

void startInterface(SensorFactory &factory, SensorConfigurations &sensorConfigs) {
  webResources.factory = &factory;
  webResources.sensorConfigurations = &sensorConfigs;

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS MOUNT FAILED!");
  }

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