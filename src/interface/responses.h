#pragma once

#include <ESPAsyncWebServer.h>

void internalServerErrorResponse(AsyncWebServerRequest * request, String text) {
    Serial.print("ERROR: ");
    Serial.println(text);
    request->send(500, "text/html", text);
}

