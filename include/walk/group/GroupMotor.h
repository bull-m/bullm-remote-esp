#ifndef ESP32S3_GroupMotor_H
#define ESP32S3_GroupMotor_H

#define GROUP_TYPE_NO ""
#define GROUP_TYPE_2D1PWM "2D1Pwm"
#define GROUP_TYPE_2EN2PWM "2En2Pwm"
#define GROUP_TYPE_2PWM "2Pwm"
#define GROUP_TYPE_1PWM "1Pwm"
#include "../basic/BasicGroup.h"

// TODO 限位器的实现
// TODO PID控制的实现
class GroupMotor : public BasicGroup {
public:
    GroupMotor();
    ~GroupMotor() override;
    BasicOutput* forward = nullptr;
    BasicOutput* back = nullptr;
    BasicOutput* pwm = nullptr;
    BasicOutput* forward_pwm = nullptr;
    BasicOutput* back_pwm = nullptr;
    bool reversal;
    bool auto_brake; // 是否自动刹车
    int min_speed = 0;
    int max_speed = 255;
    String type = GROUP_TYPE_NO;
    unsigned long last_update_time = 0; // 最后的更新时间，不等于0表示需要检查超时
    void attach(BasicOutput *_pwm);
    void attach(BasicOutput *_forward, BasicOutput *_back);
    void attach(BasicOutput *_forward, BasicOutput *_back, BasicOutput *_pwm);
    void attach(BasicOutput *_forward, BasicOutput *_back, BasicOutput *forward_pwm, BasicOutput *back_pwm);

    void attach() override;
    void detach() override;
    int read() override;
    void write(int _value) override {};
    void write2byte(int _value, int _value2) override;
    void reset(bool force = false) override; // 重置速度
    void checkTimeout(); // 检查是否超时
private:
    const unsigned long UDP_TIMEOUT = 1000;
};

#endif //ESP32S3_GroupMotor_H
