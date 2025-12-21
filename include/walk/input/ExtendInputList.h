/**
 * 表示一个输入的集合
 */
#include <Arduino.h>

#ifndef ESP32S3_EXTEND_INPUT_H
#define ESP32S3_EXTEND_INPUT_H

class ExtendInputList {
public:
    virtual void attachInput(uint8_t index) = 0;
    virtual int readInput(uint8_t index) = 0; // 读取INPUT值
};
#endif //ESP32S3_EXTEND_INPUT_H
