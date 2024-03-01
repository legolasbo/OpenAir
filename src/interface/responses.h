#ifndef INTERFACE_RESPONSES_H
#define INTERFACE_RESPONSES_H

#include <ESPAsyncWebServer.h>

void internalServerErrorResponse(AsyncWebServerRequest * request, String text) {
    Serial.print("ERROR: ");
    Serial.println(text);
    request->send(500, "text/html", text);
}

#endif
