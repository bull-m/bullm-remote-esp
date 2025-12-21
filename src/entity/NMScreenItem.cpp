#include "entity/NMScreenItem.h"
#include <WiFi.h>


void NMScreenItem::setDisplay(Adafruit_SSD1306 *_display) {
    display = _display;
}

void NMScreenItem::setType(int _type) {
    type = _type;
}

void NMScreenItem::setWidth(int _width) {
    width = _width;
}

void NMScreenItem::setPrefix(String _prefix) {
    prefix = std::move(_prefix);
}

void NMScreenItem::setDelay(int _delay) {
    delay = _delay;
}

void NMScreenItem::setSize(int _size) {
    size = _size;
    if (size > 3) {
        size = 3;
    }
    if (size < 1) {
        size = 1;
    }
    size = _size;
    if (size == 1) {
        height = 8 + 4;
    } else if (size == 2) {
        height = 16 + 4;
    } else if (size == 3) {
        height = 24 + 4;
    }
}

void NMScreenItem::render(int16_t x, int16_t y) {
    if (millis() - this->ms >= this->delay) { // 每隔500毫秒执行一次
        this->ms = millis();
        switch (this->type) {
            case TYPE_WIFI:
                this->content = WiFi.SSID();
                break;
            case TYPE_RSSI:
                this->content = String(WiFi.RSSI()) + "db";
                break;
            case TYPE_LOCAL:
                this->content = WiFi.localIP().toString();
                break;
            case TYPE_LINK:
                this->content = "...";
                break;
            default:
                this->content = "";
        }
        this->content = this->prefix + this->content;
    }

    this->display->setTextSize(this->size);      // Normal 1:1 pixel scale
    this->display->setTextColor(SSD1306_WHITE);
    this->display->setCursor(int16_t(x + 5), y);
    this->display->println(this->content);
}
