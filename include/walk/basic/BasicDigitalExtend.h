

#ifndef ESP32S3_BASICDIGITALEXTEND_H
#define ESP32S3_BASICDIGITALEXTEND_H

#include "../digital/DigitalByExtend.h"

class DigitalByExtend;

class BasicDigitalExtend {
public:
    BasicDigitalExtend() = default;
    virtual ~BasicDigitalExtend() = default;
    virtual void attachDigital(uint8_t pin) = 0;
    virtual void detachDigital(uint8_t pin) = 0;
    virtual void writeDigital(uint8_t pin, uint8_t value) = 0;
    virtual bool readDigital(uint8_t pin) = 0; // 读取OUTPUT值
    DigitalByExtend* useDigital(uint8_t pin); // 获取一个输出
};


#endif //ESP32S3_BASICDIGITALEXTEND_H
