#include "walk/walk.h"
#include "walk/servo/ServoIO.h"
#include "walk/servo/ServoByExtend.h"
#include "walk/servo/ServoByPwm.h"
// 舵机初始化
void InitServo() {
    auto servos_json = OptionsGetJson(OPTIONS_WALK_SERVOS);
    int index = 0;
    for (JsonObject item: servos_json->as<JsonArray>()) {
        String pin_s = item["pin"];
        uint16_t key = RemoteKey::servo(index++);
        BasicServo *servo = nullptr;
        if (pin_s.startsWith(PREFIX_PIN)) {
            auto *servo_io = new ServoIO();
            servo_io->io_pin = pin_s.substring(strlen(PREFIX_PIN)).toInt();
            servo_io->setHz(item["hz"].isNull() ? SERVO_hz_DEFAULT : item["hz"]);
            servo_io->attach();
            servo = servo_io;
        } else if (pin_s.startsWith(PREFIX_EXTEND)) {
            auto *extend = getExtendById(pin_s);
            if (!extend) continue;
            auto *extend_servo = dynamic_cast<BasicServoExtend *>(extend);
            if (!extend_servo) continue;
            servo = extend_servo->useServo(extend->id_to_pin[pin_s]);
        } else if (pin_s.startsWith(PREFIX_PWM)) {
            auto *output = getOutputById(pin_s);
            if (!output) continue;
            auto *pwm = dynamic_cast<BasicPwm *>(output);
            if (!pwm) continue;
            auto *servo_by_pwm = new ServoByPwm();
            servo_by_pwm->attach(pwm);
            servo = servo_by_pwm;
        }
        if (!servo) continue;
        servo->min_angle = item["min"].isNull() ? 0 : item["min"];
        servo->max_angle = item["max"].isNull() ? 180 : item["max"];
        servo->min_pulse = item["pulse_min"].isNull() ? SERVO_uS_LOW_DEFAULT : item["pulse_min"];
        servo->max_pulse = item["pulse_max"].isNull() ? SERVO_uS_HIGH_DEFAULT : item["pulse_max"];
        servo->id = item["id"].as<String>();
        servo->link_reset = item["reset"].as<bool>();
        servo->default_value = item["def"].isNull() ? 255 : item["def"].as<int>();
        servo->reset(true); // 强制回到默认值
        outputs[key] = servo;
    }
}
