
#ifndef ESP32S3_DigitalByExtend_H
#define ESP32S3_DigitalByExtend_H
#include "../basic/BasicOutput.h"
#include "../basic/BasicDigital.h"
#include "../basic/BasicDigitalExtend.h"

class BasicDigitalExtend;

class DigitalByExtend: public BasicDigital {
public:
    BasicDigitalExtend *extend;
    uint8_t pin;
    void attach()override; //连接引脚
    void write(int state) override; // 写入
    void write2byte(int _value, int _value2) override;
    int read() override; // 读取
    void detach() override;

};

#endif //ESP32S3_DigitalByExtend_H
