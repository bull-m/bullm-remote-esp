
#ifndef ESP32S3_BasicGroup_H
#define ESP32S3_BasicGroup_H
#include <Arduino.h>
#include "./BasicOutput.h"

class BasicGroup: virtual public BasicOutput {
public:
    BasicGroup(){
        type = OUTPUT_TYPE_GROUP;
    }
};
#endif //ESP32S3_BasicGroup_H
