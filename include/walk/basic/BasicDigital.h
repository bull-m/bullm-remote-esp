
#ifndef ESP32S3_BasicDigital_H
#define ESP32S3_BasicDigital_H
#include <Arduino.h>
#include "./BasicOutput.h"

class BasicDigital: virtual public BasicOutput {
public:
    BasicDigital(){
        type = OUTPUT_TYPE_DIGITAL;
    }
};
#endif //ESP32S3_BasicDigital_H
