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
    void write(int *_data, int length) override;
    // ** PWM基础方法 **
    void writePwm(int _value) override;
    void writePwmHigh(int _value) override;
};


#endif //ESP32S3_PwmByExtend_H
