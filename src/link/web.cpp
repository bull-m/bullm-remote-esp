#include "link/link.h"
#include "model/options.h"
#include "index_html.h"


void WebInit(){
    server.on("/ping", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "BULLM-REMOTE");
    });
    server.on("/api/info", HTTP_GET, [](AsyncWebServerRequest *request) {
        JsonDocument json;
        json["ip"] = WiFi.localIP().toString();
        json["mac"] = WiFi.macAddress();
        json["type"] = INFO_TYPE;
        json["version"] = INFO_VERSION;
        json["hardware_version"] = INFO_HARDWARE_VERSION;
        json["esp_reset_reason"] = esp_reset_reason();
        String jsonString = "";
        serializeJson(json, jsonString);
        request->send(200, "application/json", jsonString);
        json.clear();
    });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", index_html);
    });
    // 获取wifi
    server.on("/api/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", OptionsGet(OPTIONS_WIFi));
    });
    // 设置wifi
    server.on("/api/wifi-set", HTTP_GET, [](AsyncWebServerRequest *request) {
        String data = request->getParam("data")->value();
        if (data != "") {
            OptionsSet(OPTIONS_WIFi, data.c_str());
            request->send(200, "application/json", "success");
        } else {
            request->send(501, "text/plain", "error");
        }
    });
    //First request will return 0 results unless you start scan from somewhere else (loop/setup)
    //Do not request more often than 3-5 seconds
    server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = "[";
        int n = WiFi.scanComplete();
        if(n == -2){
            WiFi.scanNetworks(true);
        } else if(n){
            for (int i = 0; i < n; ++i){
                if(i) json += ",";
                json += "{";
                json += "\"rssi\":"+String(WiFi.RSSI(i));
                json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
                json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
                json += ",\"channel\":"+String(WiFi.channel(i));
                json += ",\"secure\":"+String(WiFi.encryptionType(i));
                json += "}";
            }
            WiFi.scanDelete();
            if(WiFi.scanComplete() == -2){
                WiFi.scanNetworks(true);
            }
        }
        json += "]";
        request->send(200, "application/json", json);
        json = String();
    });
}