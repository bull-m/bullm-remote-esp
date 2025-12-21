#include <Arduino.h>
#include "../basic/BasicPwmExtend.h"
#include "../basic/BasicPINExtend.h"
#include "../pwm/PwmByExtend.h"
#include "../digital/DigitalByExtend.h"
#include "../servo/ServoByExtend.h"

#ifndef ESP32S3_I2C_EXTEND_PCA9555_H
#define ESP32S3_I2C_EXTEND_PCA9555_H

// 寄存器地址
#define PCA9555_INPUT0      0x00
#define PCA9555_INPUT1      0x01
#define PCA9555_OUTPUT0     0x02
#define PCA9555_OUTPUT1     0x03
#define PCA9555_POLARITY0   0x04
#define PCA9555_POLARITY1   0x05
#define PCA9555_CONFIG0     0x06
#define PCA9555_CONFIG1     0x07

class I2CExtendPCA9555: public BasicPINExtend, public BasicDigitalExtend{
private:
    uint8_t _readRegister(uint8_t reg);
    bool _writeRegister(uint8_t reg, uint8_t data);
    uint8_t _readPort(uint8_t port);
    uint8_t _readPin(uint8_t port, uint8_t pin);
    bool _writePort(uint8_t port, uint8_t data);
    bool _writePin(uint8_t port, uint8_t pin, bool state);
    bool _setConfig(uint8_t port, uint8_t modes);
    bool _setConfigPin(uint8_t port, uint8_t pin, uint8_t mode);
    bool _setPolarity(uint8_t port, uint8_t reverses);
    bool _setPolarityPin(uint8_t port, uint8_t pin, uint8_t reverse);
public:
    I2CExtendPCA9555() = default;
    ~I2CExtendPCA9555() override;
    uint8_t address{}; // 地址
    int hz{};
    void attach() override {}; // 初始化
    void detach() override {}; // 脱离
    void reset() override {};

    void attachDigital(uint8_t index) override;
    void detachDigital(uint8_t index) override;
    void writeDigital(uint8_t index, uint8_t value) override;
    bool readDigital(uint8_t index) override; // 读取PWM值
};
#endif //ESP32S3_I2C_EXTEND_PCA9555_H
