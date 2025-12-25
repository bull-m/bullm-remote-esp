#include "walk/servo/ServoByExtend.h"

ServoByExtend *BasicServoExtend::useServo(uint8_t index) {
    auto* servo = new ServoByExtend();
    servo->extend = this;
    servo->index = index;
    attachServo(index); // 连接到servo
    return servo;
}

void ServoByExtend::write(int *_data, int length) {
    int _angle = _data[0];
    if (_angle == 0xFF) {
        value = _angle;
        detach();
        return;
    }
    if (_angle > max_angle) {
        _angle = max_angle;
    }
    if (_angle < min_angle) {
        _angle = min_angle;
    }
    value = _angle;
    double pulse = ((double)value * ((double)max_pulse - (double)min_pulse)) / 180 + (double)min_pulse;
    extend->writeServo(index, pulse);
}

int ServoByExtend::read() {
    return value;
}

// 卸载
void ServoByExtend::detach() {
    extend->detachServo(index);
}

void ServoByExtend::attach() {
    extend->attachServo(index);
}