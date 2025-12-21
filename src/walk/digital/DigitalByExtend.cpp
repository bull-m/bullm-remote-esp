
#include "walk/digital/DigitalByExtend.h"

DigitalByExtend *BasicDigitalExtend::useDigital(uint8_t pin) {
    auto* pwm = new DigitalByExtend();
    pwm->extend = this;
    pwm->pin = pin;
    attachDigital(pin); // 连接到pwm
    return pwm;
}

void DigitalByExtend::attach() {
    extend->attachDigital(pin);
}
int DigitalByExtend::read() {
    return extend->readDigital(pin);
}

void DigitalByExtend::detach() {
    extend->detachDigital(pin);
}

void DigitalByExtend::write(int state) {
    extend->writeDigital(pin, state);
}

void DigitalByExtend::write2byte(int _value, int _value2) {

}
