
#include "../basic/BasicPwm.h"
#include "../basic/BasicServo.h"
#include "../basic/BasicOutput.h"
#include <ESP32Servo.h>

#ifndef ESP32S3_ServoByPwm_H
#define ESP32S3_ServoByPwm_H

class ServoByPwm : public BasicServo {
public:
    ~ServoByPwm() override;
    BasicPwm* pwm = nullptr;

    void attach(BasicPwm* pwm);
    void detach() override;
    int read() override;
    void write(int _value) override;
};

#endif //ESP32S3_ServoByPwm_H
