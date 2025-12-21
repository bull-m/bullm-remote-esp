
#include <WiFi.h>
#include <lwip/netdb.h>
#include "link/link.h"
#include "main.h"
#include "model/options.h"

NMWiFiUDP udp;

IPAddress udp_serve_ip;
int udp_serve_port;

void UdpInit() {
    udp.begin(UDP_PORT);
}

// 服务器认证
void UdpServerAuth(){
    // 认证UDP
    auto network_json = OptionsGetJson(OPTIONS_NETWORK);
    auto network = network_json->as<JsonObject>();
    String host = network["ip"].as<String>();

    // 检查host是否有效
    if (host.isEmpty()) {
        Serial.println("Error: Host is empty");
        return;
    }

    // 解析IP和端口
    String ip;
    uint16_t port = 0;

    int colonIndex = host.indexOf(":");
    if (colonIndex > 0) {
        // 存在端口号
        ip = host.substring(0, colonIndex);
        String portStr = host.substring(colonIndex + 1);
        port = portStr.toInt();
    } else {
        // 不存在端口号，使用默认端口或返回错误
        ip = host;
        port = 80; // 默认端口，可根据实际需求修改
        Serial.println("Warning: Port not specified, using default port 80");
    }

    // 验证端口有效性
    if (port == 0) {
        Serial.println("Error: Invalid port number");
        return;
    }

    // 验证IP地址有效性
    if (ip.isEmpty()) {
        Serial.println("Error: Invalid IP address");
        return;
    }

    udp.beginPacket(ip.c_str(), port);
    JsonDocument json;
    json["type"] = "car";  // 小车认证
    json["token"] = network["token"].as<String>();  // token
    json["mac"] = WiFi.macAddress();  // mac
    String jsonString = "";
    serializeJson(json, jsonString);
    udp.print(jsonString);
    udp.endPacket();
    // 保存UDP目标信息
    auto pHostent = gethostbyname(ip.c_str());
    udp_serve_ip = IPAddress((const uint8_t *)(pHostent->h_addr_list[0]));
    udp_serve_port = port;

    // 输出调试信息
    Serial.print("UDP Auth sent to ");
    Serial.print(ip);
    Serial.print(":");
    Serial.println(port);
    Serial.print(":");
    Serial.println(udp_serve_ip.toString());
}
