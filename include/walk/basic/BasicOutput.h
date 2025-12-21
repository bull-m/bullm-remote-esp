//
// Created by yyz_n on 2025/9/5.
//

#ifndef ESP32S3_BASICOUTPUT_H
#define ESP32S3_BASICOUTPUT_H
#include <Arduino.h>

typedef enum {
    OUTPUT_TYPE_NO = 0,
    OUTPUT_TYPE_DIGITAL,
    OUTPUT_TYPE_PWM,
    OUTPUT_TYPE_SERVO,
    OUTPUT_TYPE_GROUP,
} OutputType;

class BasicOutput {
public:
    BasicOutput() = default;
    virtual ~BasicOutput() = default;
    String id;
    int value = -1;
    int default_value{};
    bool link_reset = false;
    OutputType type;
    unsigned long last_update_time = 0; // 最后的更新时间，不等于0表示需要检查超时
    virtual void reset(bool force) {
        if(force || link_reset){
            write(default_value);
        }
    }; // 前往默认位置(是否强制重置)
    virtual void write(int _value){}; // 写入值
    virtual void write2byte(int _value, int _value2){}; // 写入二字节数据
    virtual int read(){ return 0; }; // 读取值
    virtual void attach(){}; // 控制IO引脚的PWM
    virtual void detach(){}; // 释放资源
};
#endif //ESP32S3_BASICOUTPUT_H
