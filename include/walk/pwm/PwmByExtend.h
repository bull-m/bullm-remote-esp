//
// Created by yyz_n on 2025/9/5.
//

#ifndef ESP32S3_PwmByExtend_H
#define ESP32S3_PwmByExtend_H
#include "../basic/BasicOutput.h"
#include "../basic/BasicPwm.h"
#include "../basic/BasicPwmExtend.h"

class BasicPwmExtend;

class PwmByExtend : public BasicPwm{
public:
    BasicPwmExtend *extend{};
    uint8_t pin{};
    // ** 基础方法 **
    void attach() override;
    void detach() override;
    int read() override;
    void write(int _value) override;
    void write2byte(int _value, int _value2) override;
    // ** PWM基础方法 **
    void writePwmHigh(int _value) override;
};


#endif //ESP32S3_PwmByExtend_H
