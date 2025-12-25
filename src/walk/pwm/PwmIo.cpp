#include "walk/pwm/PwmIo.h"
#include "utils.h"

PwmIo::PwmIo() {

}

PwmIo::~PwmIo() {
    if (io_pwm) delete io_pwm;
    io_pwm = nullptr;
}

void PwmIo::attach() {
    detach();
    io_pwm = new ESP32PWM();
    io_pwm->attachPin(io_pin, hz);
}

void PwmIo::writePwm(int _pwm) {
    if (!io_pwm) return;
    if (_pwm > max_pwm) _pwm = max_pwm;
    if (_pwm < min_pwm) _pwm = min_pwm;
    if (_pwm == value) return; // 值没有改变
    value = _pwm;
    // 还没有初始化
    if (!io_pwm->attached()) io_pwm->attachPin(io_pin, hz);
    io_pwm->writeScaled(mapf(value, 0, 255, 0.0, 1.0)); // 占空比
}

void PwmIo::writePwmHigh(int _pwm) {
    if (!io_pwm) return;
    int min = map_lround(min_pwm, 0, 255, 0, 4096);
    int max = map_lround(max_pwm, 0, 255, 0, 4096);
    if (_pwm > max) _pwm = max;
    if (_pwm < min) _pwm = min;
    value = map_lround(_pwm, 0, 4096, 0, 255);
    // 还没有初始化
    if (!io_pwm->attached()) io_pwm->attachPin(io_pin, hz);
    io_pwm->writeScaled(mapf(_pwm, 0, 4096, 1.0, 0.0)); // 占空比
}


void PwmIo::write(int *_data, int length) {
    if (length == 1) {
        writePwm(_data[0]);
    } else if (length == 2) {
        writePwmHigh(merge2byte(_data[0], _data[1]));
    }
}


int PwmIo::read() {
    return value;
}

void PwmIo::detach() {
    if (!io_pwm) return;
    if (io_pwm->attached()) {
        io_pwm->detachPin(io_pin);
    }
}

void PwmIo::setHz(int _hz) {
    hz = _hz;
    if (!io_pwm) return;
    if (io_pwm->attached()) {
        io_pwm->detachPin(io_pin);
        io_pwm->attachPin(io_pin, hz);
    }
}
