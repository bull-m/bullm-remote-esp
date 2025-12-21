/**
 * http和websocket
 */
#include <Arduino.h>
#include "ArduinoJson.h"
#include "ESPAsyncWebServer.h"
#include "model/camera.h"
#include "model/oled.h"
#include <esp_task_wdt.h>
#include "main.h"
#include "model/options.h"
#include <WiFi.h>
#include "link/link.h"


AsyncWebServer server(HTTP_PORT);

bool validToken(AsyncWebServerRequest *request);

void HttpInit() {
    // 验证token中间件
    server.addMiddleware([](AsyncWebServerRequest *request, const ArMiddlewareNext &next) {
        if (request->url() == "/" ||
            request->url() == "/ping" ||
            validToken(request)) {
            next();
            return;
        }
    });
    // 初始化WebSocket
    WsInit();
    // 初始化Web
    WebInit();


    // 默认允许跨域
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->method() == HTTP_OPTIONS) {
            request->send(200);
        } else {
            request->send(404);
        }
    });
    server.begin();
}


bool validToken(AsyncWebServerRequest *request) { // count 是否验证连接数量
    // AP模式下直接通过验证，因为wifi密码就是token
    if (WiFiClass::getMode() == WIFI_MODE_AP) {
        return true;
    }
    String yes_token = OptionsGet(OPTIONS_TOKEN); // 获取存储的token
    if (yes_token == "") { // token为空，直接通过验证
        return true;
    }
    // 检查token参数
    if (!request->hasParam("token")) {
        request->send(401, "text/plain", "error");
        return false;
    }
    String token = request->getParam("token")->value();
    if (token != yes_token) {
        request->send(401, "text/plain", "error");
        return false;
    }
    return true;
}

void HttpLoop() {
}