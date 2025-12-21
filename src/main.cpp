/**
 * 版权所有 © 2025 牛明工作室 / yy祝。保留所有权利。
 * SPDX-License-Identifier: MIT
 * 根据 MIT 许可证（MIT License）授权。
 */
#include <WiFi.h>
#include <Preferences.h>
#include "main.h"
#include <esp_system.h>
#include <nvs_flash.h>
#include "model/adc.h"
#include "model/rgb.h"
#include "model/camera.h"
#include "walk/walk.h"
#include "model/oled.h"
#include "model/i2c.h"
#include "model/wifi.h"
#include "link/link.h"


void reset() {
    // 重置逻辑
    //将引脚1设置为输入上拉模式
    pinMode(PIN_RESET, INPUT_PULLUP);
    int i = 0;
    rgb_loop_one(0xf3cc0c);
    while (digitalRead(PIN_RESET) == LOW) {
        if (i >= 20) { // 10秒内一直按下复位按钮则复位
            OledText("reseting...");
            rgb_loop_one(0x2ecc71);
            for (size_t j = 0; j < 30; j++) {
                digitalWrite(PIN_TIP_LED, i % 2);
                delay(150);
            }
            Preferences prefs;
            prefs.begin("options");
            prefs.clear();
            prefs.end();
            rgb_color_all(0x1abc9c);
            OledText("reset success !");
            delay(500);
            ESP.restart();
            return;
        }
        OledText("reset by " + String(10 - i / 2) + "s");
        delay(500);
        digitalWrite(PIN_TIP_LED, i % 2);
        i++;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    // 指示灯
    pinMode(PIN_TIP_LED, OUTPUT);
    digitalWrite(PIN_TIP_LED, HIGH);
    // 电机驱动
    pinMode(PIN_MOTOR_ENABLE, OUTPUT);
    digitalWrite(PIN_MOTOR_ENABLE, LOW);
    // CAM_3V3
    pinMode(PIN_CAM_ENABLE, OUTPUT);
    digitalWrite(PIN_CAM_ENABLE, HIGH);

    esp_err_t ret;

    ret = nvs_flash_init(); /* 初始化NVS */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    I2cInit(); // 初始化i2c
    OledInit(); // 初始化oled
    RgbInit(); // 初始化rgb
    OledText("-> start");

    reset(); // 复位逻辑

    rgb_loop_one(0x008000);
    OledText("link wifi");
    WifiInit(); // 初始化wifi
    if (WiFiClass::getMode() == WIFI_AP) {
        OledText("wifi: AP Mode");
        OledText("ip: " + WiFi.softAPIP().toString());
    } else {
        OledText("wifi: " + WiFi.SSID());
        OledText("ip: " + WiFi.localIP().toString());
    }
    // 初始化udp服务
    UdpInit();
    // 初始化http的处理
    HttpInit();
    // 启动相机任务
    CameraInit();
    // 初始化adc任务
    AdcInit();
    // 初始化运动
    WalkInit();

    // 关闭指示灯
    digitalWrite(PIN_TIP_LED, LOW);
    OledText("success !");
    delay(1000);

    esp_reset_reason_t reason = esp_reset_reason();
    if (reason != ESP_RST_POWERON) {
        OledText("error restart");
        delay(3000);
    }
    rgb_off();
    OledSetInfoMode(false);
    Serial.println("\n=== 初始化完成 ===");
}

uint8_t incomingPacket[537];
uint16_t seq = 0; // 序列号

void loop() {
    ws.cleanupClients(1); // 只允许一个
    WifiCheckStatus(); // 定时检查wifi状态
    HttpLoop();
    WalkCheckTimeout();
    // HandleLoop();
    if (udp.parsePacket()) {
        int len = udp.read(incomingPacket, 536);
        if (len > 0) {
//            incomingPacket[len] = 0;
//          for (int i = 0; i < len; i++) {
//            Serial.print(incomingPacket[i]);
//            Serial.print(" ");
//          }
//          Serial.println("");

            // 验证长度
            uint16_t data_len = (incomingPacket[len - 3] << 8) | incomingPacket[len - 2];
            if (data_len != len - 5) {
                return;
            }

            // 验证序列号，确保数据是按顺序到达的
            uint16_t next_seq = (incomingPacket[len - 5] << 8) | incomingPacket[len - 4];
            if (next_seq <= seq) { // 接受大于当前seq的序列号
                // 允许回绕情况：当前seq接近最大值，而next_seq从0开始
                if (!(seq >= 59900 || next_seq < 100)) {
                    Serial.println("序列号错误");
                    return; // 丢弃
                }
            }

            // 校验码
            uint8_t check = incomingPacket[len - 1];
            // 校验码验证
            uint8_t calcCheck = 0;
            // 校验码校验的数据范围为0 ~ (len - 1) （即不包括校验码本身）
            for (int i = 0; i < len - 1; i++) {
                calcCheck += incomingPacket[i];
            }
            if (calcCheck != check) {
//                Serial.println("校验码错误");
                return;
            }
            // 更新序列号
            seq = next_seq;
            // 处理运动
            WalkHandle(incomingPacket, data_len);
        }
    }
}