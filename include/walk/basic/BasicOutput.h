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
    OutputType type = OUTPUT_TYPE_NO;
    unsigned long last_update_time = 0; // 最后的更新时间，不等于0表示需要检查超时
    virtual void reset(bool force) {
        if(force || link_reset){
            write(default_value);
        }
    }; // 前往默认位置(是否强制重置)
    void write(int _value){
        int v[1] = {_value}; // 参数归一
        write(v, 1); // 参数归一
    }; // 写入值
    virtual void write(int *_data, int length){}; // 写入多个值
    virtual int read(){ return 0; }; // 读取值
    virtual void attach(){}; // 控制IO引脚的PWM
    virtual void detach(){}; // 释放资源
};
#endif //ESP32S3_BASICOUTPUT_H
