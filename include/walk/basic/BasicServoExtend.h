#ifndef ESP32S3_BASICSERVOEXTEND_H
#define ESP32S3_BASICSERVOEXTEND_H
#include "../servo/ServoByExtend.h"
#include "./BasicServo.h"

class ServoByExtend;

class BasicServoExtend {
public:
    BasicServoExtend() = default;
    virtual ~BasicServoExtend() = default;
    virtual void attachServo(uint8_t index) = 0;
    virtual void detachServo(uint8_t index) = 0;
    /**
     * @param index
     * @param pulse 脉宽
     */
    virtual void writeServo(uint8_t index, double pulse) = 0;
    virtual int readServo(uint8_t index) = 0; // 读取OUTPUT值
    ServoByExtend* useServo(uint8_t index); // 获取一个输出
};
#endif //ESP32S3_BASICSERVOEXTEND_H
