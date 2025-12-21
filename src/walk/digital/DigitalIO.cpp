
#include "walk/digital/DigitalIO.h"

void DigitalIO::attach() {
    pinMode(io_pin, OUTPUT);
}

void DigitalIO::write(int state) {
    digitalWrite(io_pin, state > 0 ? 1 : 0);
}

int DigitalIO::read() {
    return digitalRead(io_pin);
}

void DigitalIO::detach() {}