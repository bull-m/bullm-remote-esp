#include "walk/pwm/PwmByExtend.h"
#include "utils.h"

PwmByExtend *BasicPwmExtend::usePwm(uint8_t pin){
    auto pwm = new PwmByExtend();
    pwm->extend = this;
    pwm->pin = pin;
    attachPwm(pin); // 连接到pwm
    return pwm;
}

void PwmByExtend::attach() {
    extend->attachPwm(pin);
}

void PwmByExtend::writePwm(int _pwm) {
    if(_pwm > max_pwm) _pwm = max_pwm;
    if(_pwm < min_pwm) _pwm = min_pwm;
//    if(_pwm == value) return; // 值没有改变
    value = _pwm;
    extend->writePwm(pin, _pwm);
}

void PwmByExtend::writePwmHigh(int _pwm) {
    int min = map_lround(min_pwm, 0, 255, 0, 4096);
    int max = map_lround(max_pwm, 0, 255, 0, 4096);
    if(_pwm > max) _pwm = max;
    if(_pwm < min) _pwm = min;
    value = map_lround(_pwm, 0, 4096, 0, 255);
    extend->writePwmTall(pin, _pwm);
}


void PwmByExtend::write(int *_data, int length) {
    if (length == 1) {
        writePwm(_data[0]);
    } else if (length == 2) {
        writePwmHigh(merge2byte(_data[0], _data[1]));
    }
}

int PwmByExtend::read() {
    return value; // TODO 从扩展版读取
//    return extend->readPwm(pin);
}

void PwmByExtend::detach() {
    extend->detachPwm(pin);
}

