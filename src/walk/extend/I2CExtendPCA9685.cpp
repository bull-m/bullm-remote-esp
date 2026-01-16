#include <walk/extend/I2CExtendPCA9685.h>
#include <utils.h>
#include "model/i2c.h"


I2CExtendPCA9685::~I2CExtendPCA9685() {

}
// 在PCA9555读单个寄存器
uint8_t I2CExtendPCA9685::_read(uint8_t reg) {
    uint8_t buffer[1] = {reg};
    bool res = I2cWriteThenRead(address, buffer, 1, buffer, 1);
    if (!res) {
        return 0;
    }
    return buffer[0];
}
// 向PCA9555写单个寄存器
bool I2CExtendPCA9685::_write(uint8_t reg, uint8_t data) {
    uint8_t buffer[2] = {reg, data};
    return I2cWrite(address, buffer, 2);
}
bool I2CExtendPCA9685::_reset(){
    auto res = _write(PCA9685_MODE1, MODE1_RESTART);
    delay(10);
    return res;
}
bool I2CExtendPCA9685::_setFreq(uint16_t freq) {
    if (freq < 1)
        freq = 1;
    if (freq > 3500)
        freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)
    float prescaleval = ((PCA9685_FREQUENCY_OSCILLATOR / (freq * 4096.0)) + 0.5) - 1;
    if (prescaleval < PCA9685_PRESCALE_MIN)
        prescaleval = PCA9685_PRESCALE_MIN;
    if (prescaleval > PCA9685_PRESCALE_MAX)
        prescaleval = PCA9685_PRESCALE_MAX;
    prescale = (uint8_t) prescaleval;

    uint8_t oldmode = _read(PCA9685_MODE1);
    uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // 睡眠
    _write(PCA9685_MODE1, newmode);     //睡眠
    _write(PCA9685_PRE_SCALE, prescale); // 设置预分频器
    _write(PCA9685_MODE1, oldmode);
    delay(5);
    // 启用自动增量
    return _write(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);
}

/*!
 *  @brief  Sets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @param  on At what point in the 4096-part cycle to turn the PWM output ON
 *  @param  off At what point in the 4096-part cycle to turn the PWM output OFF
 *  @return 0 if successful, otherwise 1
 */
bool I2CExtendPCA9685::_setPwm(uint8_t pin, uint16_t on, uint16_t off) {
    uint8_t buffer[5];
    buffer[0] = PCA9685_LED_START + 4 * pin;
    buffer[1] = on;
    buffer[2] = on >> 8;
    buffer[3] = off;
    buffer[4] = off >> 8;

    return I2cWrite(address, buffer, 5);
}
/**
 * 设置占空比 0 ~ 255
 * @param pin
 * @param duty_cycle 占空比
 * @return
 */
bool I2CExtendPCA9685::_setPwm(uint8_t pin, uint8_t val) {
    return _setPwmHigh(pin, val * 4095 / 255);
}

/**
 * 设置占空比 0 ~ 4095
 * @param pin
 * @param duty_cycle 占空比
 * @return
 */
bool I2CExtendPCA9685::_setPwmHigh(uint8_t pin, uint16_t val) {
    // 范围0和4095之间
    if (val > 4095) {
        val = 4095;
    }
    if (val == 0) {
        return _setPwm(pin, 0, 4096); // 0%的占空比
    } else if (val == 4095) {
        return _setPwm(pin, 4096, 0); // 100%的占空比
    } else {
        return _setPwm(pin, 0, val);
    }
}
uint16_t I2CExtendPCA9685::_getPWM(uint8_t num) {
    uint8_t buffer[2] = {uint8_t(PCA9685_LED_START + 4 * num), 0};
    I2cWriteThenRead(address, buffer, 1, buffer, 2);
    return uint16_t(buffer[0]) | (uint16_t(buffer[1]) << 8);
}

void I2CExtendPCA9685::attach() {
    _reset(); // 重新启动PCA9685
    _setFreq(hz);
}


void I2CExtendPCA9685::writePwm(uint8_t index, uint8_t value) {
    _setPwm(index, value);
}

void I2CExtendPCA9685::writePwmTall(uint8_t index, uint16_t value) {
    _setPwmHigh(index, value);
}

int I2CExtendPCA9685::readPwm(uint8_t index) {
    return map_lround(_getPWM(index), 0, 4096, 0, 255);
}

void I2CExtendPCA9685::writeDigital(uint8_t index, uint8_t value) {
    writePwmTall(index, value ? 4096 : 0);
}

bool I2CExtendPCA9685::readDigital(uint8_t index) {
    return _getPWM(index) >= 4095;
}

void I2CExtendPCA9685::writeServo(uint8_t index, double pulse) {
//    // pwm的频率
//    int e_hz = hz;
//    // 一个周期的脉冲宽度(μs)
//    double pulse_width = 1000000.0 / e_hz;
//    // 计算占空比
//    double duty_cycle = pulse / pulse_width;
//    // 计算占空比值
//    int duty_cycle_value = static_cast<int>(std::round(duty_cycle * 4096));
//    Serial.printf("PCA9685: Pulse: %f, Duty Cycle: %f, Duty Cycle Value: %d\n", pulse, duty_cycle, duty_cycle_value);
//    _setPwmHigh(index, duty_cycle_value);

    // 根据 Prescaler 计算占空比
    // 每个计数单位的时间(ns)
    float tick_time_ns = (prescale + 1) * 1000000000.0f / PCA9685_FREQUENCY_OSCILLATOR;
    // 将输入的微秒转换成 ticks 数量
    auto off_tick = (uint16_t)(pulse * 1000 / tick_time_ns);
    // 确保不超过最大值
    if(off_tick > 4095) off_tick = 4095;
    _setPwm(index, 0, off_tick);
}

int I2CExtendPCA9685::readServo(uint8_t index) {
    return 0;
}

void I2CExtendPCA9685::detachServo(uint8_t index) {
    _setPwmHigh(index, 0);
}

