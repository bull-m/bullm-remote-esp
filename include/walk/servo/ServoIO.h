
#include "../basic/BasicOutput.h"
#include "../basic/BasicServo.h"
#include <ESP32Servo.h>

#ifndef ESP32S3_SERVOIO_H
#define ESP32S3_SERVOIO_H

class ServoIO : public BasicServo {
public:
    ~ServoIO() override;
    uint8_t io_pin;
    Servo *io_servo = nullptr;
    int hz = SERVO_hz_DEFAULT;
    void setHz(int _hz);
    void attach() override;
    void detach() override;
    int read() override;
    void write(int _value) override;
};

#endif //ESP32S3_SERVOIO_H
