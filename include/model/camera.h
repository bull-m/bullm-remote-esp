#include <WiFi.h>
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>

#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#define PWDN_GPIO_NUM  40
#define RESET_GPIO_NUM 42
// #define XCLK_GPIO_NUM  7
#define XCLK_GPIO_NUM  48
// #define SIOD_GPIO_NUM  4
// #define SIOC_GPIO_NUM  5
#define SIOD_GPIO_NUM  1
#define SIOC_GPIO_NUM  2

#define Y2_GPIO_NUM 11
// #define Y3_GPIO_NUM 9
#define Y3_GPIO_NUM 46
#define Y4_GPIO_NUM 8
#define Y5_GPIO_NUM 10
#define Y6_GPIO_NUM 12
#define Y7_GPIO_NUM 14
// #define Y8_GPIO_NUM 6
#define Y8_GPIO_NUM 47
#define Y9_GPIO_NUM 38

#define VSYNC_GPIO_NUM 41
#define HREF_GPIO_NUM  39
#define PCLK_GPIO_NUM  13


void CameraStop();

void CameraInit();

JsonDocument * CameraHandle(JsonDocument &data);



