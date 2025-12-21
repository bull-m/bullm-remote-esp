#include <walk/extend/I2CExtendPCA9555.h>
#include "model/i2c.h"


// 在PCA9555读单个寄存器
uint8_t I2CExtendPCA9555::_readRegister(uint8_t reg) {
    uint8_t buffer[1] = {reg};
    bool res = I2cWriteThenRead(address, buffer, 1, buffer, 1);
    if(!res){
        return 0;
    }
    return buffer[0];
}

// 向PCA9555写单个寄存器
bool I2CExtendPCA9555::_writeRegister(uint8_t reg, uint8_t data) {
    uint8_t buffer[2] = {reg, data};
    return I2cWrite(address, buffer, 2);
}

// 读取一个端口
uint8_t I2CExtendPCA9555::_readPort(uint8_t port) {
    if (port > 1) return 0;
    return _readRegister(port == 0 ? PCA9555_INPUT0 : PCA9555_INPUT1);
}

/**
 * 读取一个引脚
 * @param port 端口号
 * @param pin 引脚号
 * @param isNoCache 是否不使用缓存
 * @return 引脚状态
 */
uint8_t I2CExtendPCA9555::_readPin(uint8_t port, uint8_t pin) {
    if (pin > 7) return 0;
    uint8_t input = _readPort(port);
    return (input >> pin) & 0x01;
}

/**
 * 写一个端口
 * @param port 端口号
 * @param data 数据
 * @return 是否成功
 */
bool I2CExtendPCA9555::_writePort(uint8_t port, uint8_t data) {
    if (port > 1) return false;
    uint8_t reg = port == 0 ? PCA9555_OUTPUT0 : PCA9555_OUTPUT1;
    return _writeRegister(reg, data);
}

/**
 * 写一个引脚
 * @param port 端口号
 * @param pin 引脚号
 * @param state 引脚状态
 * @return 是否成功
 */
bool I2CExtendPCA9555::_writePin(uint8_t port, uint8_t pin, bool state) {
    if (port > 1 || pin > 7) return false;
    uint8_t reg = port == 0 ? PCA9555_OUTPUT0 : PCA9555_OUTPUT1;
    uint8_t output = _readRegister(reg);
    if (state) {
        output |= (1 << pin);  // 置位输出高电平
    } else {
        output &= ~(1 << pin); // 清零输出低电平
    }
    return _writePort(port, output);
}


/**
 * 设置引脚方向 (0 = 输出, 1 = 输入)
 * @param port 端口号
 * @param config 引脚配置
 * @return 是否成功
 */
bool I2CExtendPCA9555::_setConfig(uint8_t port, uint8_t modes) {
    if (port > 1) return false;
    uint8_t reg = port == 0 ? PCA9555_CONFIG0 : PCA9555_CONFIG1;
    return _writeRegister(reg, modes);
}

/**
 * 配置引脚方向 (0 = 输出, 1 = 输入)
 * @param port 端口号
 * @param pin 引脚号
 * @param mode 引脚模式
 * @return 是否成功
 */
bool I2CExtendPCA9555::_setConfigPin(uint8_t port, uint8_t pin, uint8_t mode) {
    if (port > 1 || pin > 7) return false;
    uint8_t reg = (port == 0) ? PCA9555_CONFIG0 : PCA9555_CONFIG1;
    uint8_t config = _readRegister(reg);
    if (mode) {
        config |= (1 << pin);  // 置位输入
    } else {
        config &= ~(1 << pin); // 清零输出
    }
    return _setConfig(port, config);
}
/**
 * 是否反转输入引脚极性 (0 = 正常, 1 = 反转)
 * @param port 端口号
 * @param polarity 引脚极性
 * @return
 */
bool I2CExtendPCA9555::_setPolarity(uint8_t port, uint8_t reverses) {
    if (port > 1) return false;
    uint8_t reg = (port == 0) ? PCA9555_POLARITY0 : PCA9555_POLARITY1;
    return _writeRegister(reg, reverses);
}

/**
 * 设置引脚极性 (0 = 正常, 1 = 反转)
 * @param port 端口号
 * @param pin 引脚号
 * @param direction 引脚极性
 * @return 是否成功
 */
bool I2CExtendPCA9555::_setPolarityPin(uint8_t port, uint8_t pin, uint8_t reverse) {
    if (port > 1 || pin > 7) return false;
    uint8_t reg = (port == 0) ? PCA9555_POLARITY0 : PCA9555_POLARITY1;
    uint8_t config = _readRegister(reg);
    if (reverse) {
        config |= (1 << pin);  // 置位反转
    } else {
        config &= ~(1 << pin); // 清零正常
    }
    return _setPolarity(port, config);
}

I2CExtendPCA9555::~I2CExtendPCA9555() {
}

// 添加输出引脚
void I2CExtendPCA9555::attachDigital(uint8_t index) {
    _setConfigPin(index > 7 ? 1 : 0, index % 8, 0); // 设置为输出
}

// 脱离输出引脚
void I2CExtendPCA9555::detachDigital(uint8_t index) {
    _setConfigPin(index > 7 ? 1 : 0, index % 8, 1); // 设置为输入
}
void I2CExtendPCA9555::writeDigital(uint8_t index, uint8_t value) {
    _writePin(index > 7 ? 1 : 0, index % 8, value);
}

bool I2CExtendPCA9555::readDigital(uint8_t index) {
    return _readPin(index > 7 ? 1 : 0, index % 8);
}

