
#ifndef ESP32S3_BasicPwm_H
#define ESP32S3_BasicPwm_H
#include <Arduino.h>
#include "./BasicOutput.h"

class BasicPwm: virtual public BasicOutput{
public:
    BasicPwm(){
        type = OUTPUT_TYPE_PWM;
    }
    int hz = 50;
    int min_pwm = 0;
    int max_pwm = 255;
    virtual void writePwmHigh(int _value){}; // 写入值高精度
};
#endif //ESP32S3_BasicPwm_H
