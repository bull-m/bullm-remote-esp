#include <Arduino.h>
#include "../basic/BasicPwmExtend.h"
#include "../basic/BasicPINExtend.h"
#include "../pwm/PwmByExtend.h"
#include "../digital/DigitalByExtend.h"
#include "../servo/ServoByExtend.h"

#ifndef ESP32S3_I2C_EXTEND_PCA9586_H
#define ESP32S3_I2C_EXTEND_PCA9586_H

#define PCA9685_MODE1 0x00      // 模式寄存器1
#define PCA9685_MODE2 0x01      // 模式寄存器2

#define PCA9685_LED_START 0x06  // LED控制寄存器的开始
/**
 * // LED控制寄存器计算
 * ON_L: PCA9685_LED_START + 4 * index
 * ON_H: PCA9685_LED_START + 4 * index + 1
 * OFF_L: PCA9685_LED_START + 4 * index  + 2
 * OFF_H: PCA9685_LED_START + 4 * index  + 3
 */
#define PCA9685_PRE_SCALE 0xFE    // PWM输出频率的预分频器


#define MODE1_SLEEP 0b00010000  // 低功耗模式。振荡器关闭
#define MODE1_AI 0b00100000      // 自动增量，控制寄存器在读取或写入后会自动递增。这允许用户对寄存器进行顺序编程。
#define MODE1_RESTART 0b10000000  // 重新启动

#define MODE2_OUTDRV 0b100 // 输出驱动方式, 0: open drain, 1: totem pole

#define PCA9685_FREQUENCY_OSCILLATOR 25000000.0 // 振荡器时钟频率为25MHz：

#define PCA9685_PRESCALE_MIN 3   // 最小预标度值
#define PCA9685_PRESCALE_MAX 255 // 最大预缩放值

class I2CExtendPCA9685: public BasicPINExtend, public BasicPwmExtend, public BasicDigitalExtend, public BasicServoExtend{
private:
    bool _write(uint8_t reg, uint8_t data);
    uint8_t _read(uint8_t reg);
    void _reset();
    bool _setFreq(uint16_t freq);
    bool _setPwm(uint8_t pin, uint16_t on, uint16_t off);
    bool _setPwm(uint8_t pin, uint8_t val);
    bool _setPwmHigh(uint8_t pin, uint16_t val);
    uint16_t _getPWM(uint8_t num);
public:
    I2CExtendPCA9685() = default;
    ~I2CExtendPCA9685() override;
    uint8_t address{}; // 地址
    int hz{};
    uint8_t prescale{};
    void attach() override; // 初始化
    void detach() override {}; // 脱离
    void reset() override {};

    // PWM
    void detachPwm(uint8_t index) override {};
    void attachPwm(uint8_t index) override {};
    void writePwm(uint8_t index, uint8_t value) override;
    void writePwmTall(uint8_t index, uint16_t value) override;
    int readPwm(uint8_t index) override; // 读取PWM值

    // 数字输出
    void attachDigital(uint8_t index) override {};
    void detachDigital(uint8_t index) override {};
    void writeDigital(uint8_t index, uint8_t value) override;
    bool readDigital(uint8_t index) override; // 读取PWM值

    // 舵机
    void attachServo(uint8_t index) override {};
    void detachServo(uint8_t index) override;
    void writeServo(uint8_t index, double pulse) override;
    int readServo(uint8_t index) override; // 读取OUTPUT值
};
#endif //ESP32S3_I2C_EXTEND_PCA9586_H
