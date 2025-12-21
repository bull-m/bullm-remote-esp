

#ifndef ESP32S3_PWMIO_H
#define ESP32S3_PWMIO_H
#include "../basic/BasicOutput.h"
#include "../basic/BasicPwm.h"
#include "ESP32PWM.h"

class PwmIo : public BasicPwm{
public:
    PwmIo();
    ~PwmIo() override;

    // ** 基础方法 **
    void attach() override;
    void detach() override;
    int read() override;
    void write(int _value) override;
    void write2byte(int _value, int _value2) override;
    // ** PWM基础方法 **
    void writePwmHigh(int _value) override;
    // ** 扩展 **
    uint8_t io_pin{};
    ESP32PWM *io_pwm = nullptr; // 控制IO引脚的PWM
    void setHz(int _hz);
};
#endif //ESP32S3_PWMIO_H
