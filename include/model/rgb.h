#include <Adafruit_NeoPixel.h>
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>

void RgbInit();
void rgb_off();
void rgb_loop_one(uint32_t c);
void rgb_color_all(uint32_t c);
JsonDocument * RgbHandle(JsonDocument &data);
