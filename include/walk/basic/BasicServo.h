

#ifndef ESP32S3_BasicServo_H
#define ESP32S3_BasicServo_H
#include <Arduino.h>
#include "./BasicOutput.h"

#define SERVO_uS_LOW 500
#define SERVO_uS_HIGH 2500
#define SERVO_uS_LOW_DEFAULT 544
#define SERVO_uS_HIGH_DEFAULT 2400

#define SERVO_hz_LOW 50
#define SERVO_hz_HIGH 500
#define SERVO_hz_DEFAULT 50

class BasicServo: virtual public BasicOutput {
public:
    BasicServo(){
        type = OUTPUT_TYPE_PWM;
    }
    uint8_t min_angle = 0;
    uint8_t max_angle = 180;
    uint16_t min_pulse = SERVO_uS_LOW_DEFAULT; // 最小脉冲宽度（微秒(μs)）
    uint16_t max_pulse = SERVO_uS_HIGH_DEFAULT; // 最大脉冲宽度（微秒(μs)
};
#endif //ESP32S3_BasicServo_H
