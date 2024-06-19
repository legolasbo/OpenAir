#pragma once

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <string>
#include <WiFi.h>
#include "api.h"

class FileSystem : public API {
    private:

        void fileUploadHandler(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
            if (!index) {
                String path = request->arg("path");
                if (!path.endsWith(filename)) {
                    if (!path.endsWith("/")) {
                        path += "/";
                    }
                    path += filename;
                }
                if (!path.startsWith("/")) {
                    path = "/" + path;
                }
                if (path.startsWith("/data")) {
                    path = path.substring(5);
                }

                Log.infoln("Upload start: %s", path);
                request->_tempFile = SPIFFS.open(path, "w");
            }

            if (len) {
                request->_tempFile.write(data, len);
                Log.traceln("Writing: %s index=%d len=%d", request->_tempFile.path(), index, len);
            }

            if (final) {
                Log.infoln("Upload Complete: %s size: %d", request->_tempFile.path(),index + len);
                request->_tempFile.close();
                request->redirect("/");
            }
        }

    public:
        void configureCallbacks(AsyncWebServer &server) override {
            server.on("/api/filesystem/list", HTTP_GET, [this](AsyncWebServerRequest * request) {
                auto response = request->beginResponseStream("text/html");
                auto file = SPIFFS.open("/");

                JsonDocument files;

                int i = 0;
                while (auto f = file.openNextFile()) {
                    std::string path = std::string(f.path());
                    files[i++] = path;
                    f.close();
                }

                file.close();

                this->respondJson(files, request);
            });

            server.on("/api/filesystem/upload", HTTP_POST, 
                [this](AsyncWebServerRequest * request) {
                    request->send(200);
                },
                [this](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
                    this->fileUploadHandler(request, filename, index, data, len, final);
                }
            );

            server.on("/api/filesystem/delete", HTTP_POST, 
                [this](AsyncWebServerRequest * request) {
                    if (request->hasArg("path")) {
                        auto arg = request->arg("path");
                        if (!arg.startsWith("/")) {
                            arg = "/" + arg;
                        }
                        if (SPIFFS.remove(arg)) {
                            Log.infoln("Removed %s", arg.c_str());
                        }
                        else {
                            Log.warningln("Unable to remove %s", arg.c_str());
                        }
                    }
                    request->redirect("/filesystem");
                }
            );
        }

};
