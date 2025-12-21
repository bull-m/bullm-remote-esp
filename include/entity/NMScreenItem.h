

#ifndef NMScreenItem_H_
#define NMScreenItem_H_

#include <Arduino.h>
#include "ESP32PWM.h"
#include "Adafruit_SSD1306.h"
#include <utility>


#define TYPE_WIFI 0
#define TYPE_RSSI 1
#define TYPE_LOCAL 2
#define TYPE_LINK 3
#define TYPE_TEXT 4

class NMScreenItem {
private:
    // 上次更新的时间
    unsigned long ms = 0;
    String content;

    unsigned int width = 0; // 宽度
    unsigned int size = 1; // 占用行数

    Adafruit_SSD1306 *display;
    unsigned int type;
    unsigned int delay; // 数据刷新间隔
    String prefix;
public:
    int height = 0; // 高度
    void render(int16_t x, int16_t y); // 渲染
    void setDisplay(Adafruit_SSD1306 *_display); // 设置显示屏
    void setType(int _type); // 设置类型
    void setWidth(int _width); // 设置宽度
    void setPrefix(String _prefix); // 设置前缀
    void setDelay(int _delay); // 设置刷新间隔
    void setSize(int _size); // 设置字体大小
};

#endif // NMScreenItem_H_
