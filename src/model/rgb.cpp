#include <Adafruit_NeoPixel.h>
#include "model/rgb.h"

Adafruit_NeoPixel strip(8, 15, NEO_GRB + NEO_KHZ800);

typedef enum {
    NO,
    ALL_OFF,
    COLOR_ALL,
    LOOP_ONE, // 单色循环整个灯带
    RAINBOW, // 彩虹
} type_t;

type_t type;
int fps;
uint32_t color;
uint8_t brightness = 30;




int i = 0;

[[noreturn]] void task(void *pvParameters) {
    type_t c_type = NO;
    while (true) {
        if(brightness!= strip.getBrightness()){
            strip.setBrightness(brightness);
            strip.show();
        }
        if (c_type != type) {
            if (type == ALL_OFF)
                strip.clear();
            if (type == LOOP_ONE) {
                strip.clear();
                strip.setPixelColor(i, color);
                i++;
                if (i >= strip.numPixels())
                    i = 0;
                type = NO; // 进入下一次循环
            } else if (type == COLOR_ALL) {
                strip.fill(color);
            } else if (type == RAINBOW) {
                int step = 6000; // 间隔
                for (int j = 0; j < strip.numPixels(); j++) {
                    strip.setPixelColor(j, Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(j * step + i * step)));
                }
                i++;
                if (65536L / 100 - strip.numPixels() <= i) {
                    i = 0;
                }
                type = NO; // 进入下一次循环
            }
            strip.show();
        }
        vTaskDelay(1000 / fps / portTICK_PERIOD_MS); // 延时1秒
    }
}

void rgb_off() {
    type = ALL_OFF;
}

void rgb_loop_one(uint32_t c) {
    i = 0;
    type = LOOP_ONE;
    color = c;
}

void rgb_color_all(uint32_t c) {
    type = COLOR_ALL;
    color = c;
}

void rgb_rainbow(uint16_t step) {
    // 颜色间隔
    i = 0;
    type = RAINBOW;
}

void RgbInit() {
    strip.begin();
    strip.setBrightness(30);
    fps = 20; // 20帧率
    type = ALL_OFF;
    xTaskCreatePinnedToCore(task, "TaskRgb", 2048, nullptr, 1, nullptr, 0);
}

JsonDocument *RgbHandle(JsonDocument &json) {
    String effect = json["effect"];
    if (effect == "color_all") {
        rgb_color_all(json["color"].as<uint32_t>());
    } else if (effect == "loop_one") {
        rgb_loop_one(json["color"].as<uint32_t>());
    } else if (effect == "rainbow") {
        rgb_rainbow(json["step"].as<uint32_t>());
    } else if (effect == "off") {
        rgb_off();
    } else if (effect == "brightness") {
        brightness = json["value"].as<uint8_t>();
    }
    return new JsonDocument();
}