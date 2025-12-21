#include "walk/servo/ServoIO.h"
#include "walk/walk.h"

ServoIO::~ServoIO() {
    if (io_servo) {
        delete io_servo;
        io_servo = nullptr;
    }
}

void ServoIO::attach() {
    if (io_servo) {
        delete io_servo;
        io_servo = nullptr;
    }
    io_servo = new Servo();
    io_servo->setPeriodHertz(this->hz);
}

void ServoIO::setHz(int _hz) {
    if (_hz < SERVO_hz_LOW) _hz = SERVO_hz_LOW;
    if (_hz > SERVO_hz_HIGH) _hz = SERVO_hz_HIGH;
    hz = _hz;
    if (io_servo) {
        io_servo->setPeriodHertz(this->hz);
    }
}

void ServoIO::write(int _angle) {
    if (_angle == 0xFF) {
        value = 0xFF;
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
    // 还没有初始化
    if (!io_servo->attached()) {
        io_servo->attach(io_pin, min_pulse, max_pulse);
        io_servo->setPeriodHertz(hz);
    }
    io_servo->write(value);
}

int ServoIO::read() {
    return value;
//    if (type == SERVO_TYPE_IO) {
//        // 读取的值偏小（未知原因）这里使用上取整
//        return static_cast<int>(std::ceil(mapf(io_servo->readMicroseconds(), pulse_min, pulse_max, 0, 180)));
//    }
}

// 卸载
void ServoIO::detach() {
    value = 0xFF;
    io_servo->detach();
}
