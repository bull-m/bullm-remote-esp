#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

JsonDocument *WifiHandle(JsonDocument &data);

void WifiInit();

void WifiCheckStatus();