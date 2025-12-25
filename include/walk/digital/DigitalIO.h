#include "../basic/BasicOutput.h"
#include "../basic/BasicDigital.h"

#ifndef ESP32S3_DigitalIO_H
#define ESP32S3_DigitalIO_H
class DigitalIO : public BasicDigital{
public:
    uint8_t io_pin{};
    void attach() override;
    void detach() override;
    int read() override;
    void write(int *_data, int length) override;
};
#endif //ESP32S3_DigitalIO_H
