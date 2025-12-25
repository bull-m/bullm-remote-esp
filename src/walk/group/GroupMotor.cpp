#include "walk/group/GroupMotor.h"
#include "utils.h"

GroupMotor::GroupMotor() {}

GroupMotor::~GroupMotor() {}

void GroupMotor::attach(BasicOutput *_pwm) {
    pwm = _pwm;
    type = GROUP_TYPE_1PWM;
}

void GroupMotor::attach(BasicOutput *_forward, BasicOutput *_back) {
    forward = _forward;
    back = _back;
    type = GROUP_TYPE_2PWM;
}

void GroupMotor::attach(BasicOutput *_forward, BasicOutput *_back, BasicOutput *_pwm) {
    if(_pwm->type != OUTPUT_TYPE_PWM){
        return;
    }
    forward = _forward;
    back = _back;
    pwm = _pwm;
    type = GROUP_TYPE_2D1PWM;
}
void GroupMotor::attach(BasicOutput *_forward, BasicOutput *_back, BasicOutput *_forward_pwm, BasicOutput *_back_pwm) {
    if(_forward_pwm->type != OUTPUT_TYPE_PWM || _back_pwm->type != OUTPUT_TYPE_PWM){
        return;
    }
    forward = _forward;
    back = _back;
    forward_pwm = _forward_pwm;
    back_pwm = _back_pwm;
    type = GROUP_TYPE_2EN2PWM;
}


void GroupMotor::write(int *_data, int length) {
    walk(_data[0], _data[1]);
}

void GroupMotor::walk(int direction, int _speed) {
    last_update_time = millis(); // 刷新最后更新时间
    if(type == GROUP_TYPE_NO) return;
    _speed = _speed * (direction == 1 ? 1 : -1);
    if (_speed < 0) {
        if (_speed < -max_speed) _speed = -max_speed;
        if (_speed > -min_speed) _speed = -min_speed;
    } else if (_speed > 0) {
        if (_speed > max_speed) _speed = max_speed;
        if (_speed < min_speed) _speed = min_speed;
    }
    if(value == _speed) return; // 速度没有改变
//    bool is_reversal = (value < 0 && _speed > 0) || (value > 0 && _speed < 0);
    value = _speed;
    if(reversal) _speed = -_speed; // 反向
    try {
        if (type == GROUP_TYPE_1PWM) {
            if(pwm == nullptr) return;
            pwm->write(map_lround(_speed, -255, 255, 0, 255));
        } else if (type == GROUP_TYPE_2PWM) {
            if (forward == nullptr || back == nullptr) return;
            if(auto_brake && _speed == 0){ // 刹车
                forward->write(0xff);
                back->write(0xff);
            }else{
                forward->write(_speed > 0 ? _speed : 0);
                back->write(_speed < 0 ? -_speed : 0);
            }
        } else if (type == GROUP_TYPE_2D1PWM) {
            if (forward == nullptr || back == nullptr || pwm == nullptr) return;
            if(auto_brake && _speed == 0){ // 刹车
                forward->write(0xff);
                back->write(0xff);
                pwm->write(0);
            }else{
//                if(is_reversal){ // 死区时间
//                    forward->write(false);
//                    back->write(false);
//                    delayMicroseconds(500);
//                }
                forward->write(_speed > 0);
                back->write(_speed < 0);
                if(_speed != 0){ // 有速度
                    pwm->write(_speed < 0 ? -_speed : _speed);
                }
            }
        }else if (type == GROUP_TYPE_2EN2PWM) {
            if (forward == nullptr || back == nullptr || forward_pwm == nullptr || back_pwm == nullptr) return;
            // 刹车需要保持启用状态
            bool enable = _speed != 0 || auto_brake;
            forward->write(enable ? 0xff : 0);
            back->write(enable ? 0xff : 0);
            forward_pwm->write(_speed < 0 ? -_speed : 0);
            back_pwm->write(_speed > 0 ? _speed : 0);
        }
    } catch (std::exception &e) {
        Serial.print("Exception: ");
        Serial.println(id);
    }
}


void GroupMotor::detach() {
    walk(0, 0);
    forward = nullptr;
    back = nullptr;
    pwm = nullptr;
}



void GroupMotor::reset(bool force) {
    walk(0, 0);
}

int GroupMotor::read() {
    return value;
}

void GroupMotor::checkTimeout() {
    if(last_update_time == 0) return;
    if (millis() - last_update_time > UDP_TIMEOUT) {
        reset();
    }
}

void GroupMotor::attach() {

}
