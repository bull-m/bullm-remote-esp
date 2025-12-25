#include "walk/servo/ServoByPwm.h"
#include "walk/walk.h"

ServoByPwm::~ServoByPwm() {
//    if (pwm)
//        delete pwm;
}

void ServoByPwm::attach(BasicPwm *_pwm) {
    pwm = _pwm;
}

void ServoByPwm::write(int *_data, int length) {
    int _angle = _data[0];
    if(!pwm) return;
    if (_angle == 0xFF) {
        detach();
        return;
    }
    if (_angle > max_angle) {
        _angle = max_angle;
    }
    if (_angle < min_angle) {
        _angle = min_angle;
    }
    value = _angle;

    auto *_pwm = dynamic_cast<BasicPwm *>(pwm);
    if(_pwm == nullptr) return;
    // 一个周期的脉冲宽度(μs)
    double pulse_width = 1000000.0 / _pwm->hz;
    // 目标脉冲宽度（μs）
    double target_pulse_width = (value * (max_pulse - min_pulse)) / 180 + min_pulse;
    // 计算占空比
    double duty_cycle = target_pulse_width / pulse_width;
    // 计算占空比值
    int duty_cycle_value = static_cast<int>(std::round(duty_cycle * 4096));

    _pwm->writePwmHigh(duty_cycle_value);

}

int ServoByPwm::read() {
    return value;
}

// 卸载
void ServoByPwm::detach() {
    if(!pwm) return;
    value = 0xFF;
    pwm->writePwmHigh(0);
}

