#include "link/link.h"
#include "model/options.h"

AsyncWebSocket ws("/ws");

bool isNetwork = false;

uint32_t ws_id = 0; // WebSocket ID
uint8_t *all_data = nullptr;

// TODO 连接互联网
void linkNetworkWs() {
    if (WiFiClass::getMode() != WIFI_MODE_AP) { // AP模式下不连接互联网
        // 连接互联网
        auto network_json = OptionsGetJson(OPTIONS_NETWORK);
        auto network = network_json->as<JsonObject>();
        if (!network["ip"].isNull() && !network["token"].isNull()) {
            bool isSsl = network["ssl"];
            String url;
            url.reserve(200); // 预分配内存
            url = "ws";
            if (isSsl) url += "s";
            url += "://";
            url += network["ip"].as<String>();
            url += "/car-ws?token=";
            url += network["token"].as<String>();
            url += "&type=";
            url += INFO_TYPE;
            url += "&version=";
            url += INFO_VERSION;
            url += "&hardware_version=";
            url += INFO_HARDWARE_VERSION;
            url += "&esp_reset_reason=";
            url += String(esp_reset_reason());
            url += "&mac=";
            url += WiFi.macAddress();
            Serial.println(url);
        }
    }
}


bool validCount(AsyncWebServerRequest *request) {
    // 是否有多个连接,或者互联网控制
    if (ws.count() >= 1 || isNetwork) {
        request->send(402, "text/plain", "error");
        return false;
    }
    return true;
}


void WsInit() {
    //  linkNetworkWs(); // TODO 连接到互联网
    // 验证是否允许连接
    server.on("/api/ws/valid", HTTP_GET, [](AsyncWebServerRequest *request) {
        // 是否有多个连接
        if (validCount(request)) request->send(200, "text/plain", "success");
    });
    // 清楚其他的客户端连接
    server.on("/api/ws/clear", HTTP_GET, [](AsyncWebServerRequest *request) {
        ws.closeAll(4004, "被其他设备接管控制");
        // TODO 清空互联网的连接
        request->send(200, "text/plain", "success");
    });

    // 添加中间件
    ws.addMiddleware([](AsyncWebServerRequest *request, const ArMiddlewareNext &next) {
        if (validCount(request)) {
            next();
        }
    });

    // 事件处理
    ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                  void *arg, uint8_t *data, size_t len) {
        switch (type) {
            case WS_EVT_DATA:  // 处理 WebSocket 消息
            {
                auto *info = (AwsFrameInfo *) arg;
                // 一次传完
                if (info->index == 0 && info->len == len) {
                    if (info->opcode == WS_TEXT) {
                        HandleWsText(data, len);
                    } else {  // 字节
                        HandleWsByte(data, len);
                    }
                    return;
                }
                if (info->index == 0) {
                    all_data = new uint8_t[info->len];
                }
                // 复制数据
                for (size_t i = 0; i < len; i++) {
                    all_data[i + info->index] = data[i];
                }
                // 最后一个包
                if (info->index + len == info->len) { //
                    if (info->opcode == WS_TEXT) {
                        HandleWsText(all_data, info->len);
                    } else {  // 字节
                        HandleWsByte(all_data, info->len);
                    }
                    delete[] all_data;
                }
            }
                break;
            case WS_EVT_CONNECT:  // 有客户端建立连接
                ws_id = client->id();
                udp_serve_ip = client->remoteIP();
                udp_serve_port = 61211;
                Serial.printf("WebSocket client #%u connected from %s \n", client->id(),
                              client->remoteIP().toString().c_str());
                client->text("{\"type\":\"control-change\",\"isControl\":true}"); // 指定控制权
                HandleConnect();
                break;
            case WS_EVT_DISCONNECT:  // 有客户端断开连接
                Serial.printf("WebSocket client #%u disconnected \n", client->id());
                HandleDisconnect();
                break;
            case WS_EVT_PONG:   //  // 收到客户端对服务器发出的ping进行应答（pong消息）
                client->ping();
                break;
            case WS_EVT_ERROR:  //  // 发生错误
                WalkReset();
                break;
            case WS_EVT_PING:
                break;
        }
    });
    server.addHandler(&ws);
}


void WsAutoSend(const char *data, size_t len) {
    ws.binaryAll(data, len);
}

void WsAutoSend(JsonDocument &json) {
    String jsonString = "";
    serializeJson(json, jsonString);
    WsAutoSend(jsonString);
    json.clear();
}


void WsAutoSend(String str) {
    ws.text(ws_id, str);
}
