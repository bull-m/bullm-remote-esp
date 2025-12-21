
#ifndef ESP32S3_BASICPWMEXTEND_H
#define ESP32S3_BASICPWMEXTEND_H
#include "../pwm/PwmByExtend.h"

class PwmByExtend;

class BasicPwmExtend {
public:
    BasicPwmExtend() = default;
    virtual ~BasicPwmExtend() = default;
    virtual void attachPwm(uint8_t pin) = 0;
    virtual void detachPwm(uint8_t pin) = 0;
    virtual void writePwm(uint8_t pin, uint8_t value) = 0;
    virtual void writePwmTall(uint8_t pin, uint16_t value) = 0;
    virtual int readPwm(uint8_t pin) = 0; // 读取PWM值
    PwmByExtend* usePwm(uint8_t pin);
};


#endif //ESP32S3_BASICPWMEXTEND_H
