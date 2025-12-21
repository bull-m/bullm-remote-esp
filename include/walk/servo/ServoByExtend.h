

#ifndef ESP32S3_ServoByExtend_H
#define ESP32S3_ServoByExtend_H

#include "../basic/BasicOutput.h"
#include "../basic/BasicServoExtend.h"
#include "../basic/BasicServo.h"

class BasicServoExtend;

class ServoByExtend : public BasicServo {
public:
    BasicServoExtend *extend{};
    uint8_t index{};

    // ** 基础方法 **
    void attach() override;
    void detach() override;
    int read() override;
    void write(int _value) override;
};


#endif //ESP32S3_ServoByExtend_H
