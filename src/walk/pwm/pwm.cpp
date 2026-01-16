#include "walk/walk.h"
#include "walk/pwm/PwmIo.h"
#include "walk/basic/BasicPwmExtend.h"

// Pwm初始化
void InitPwm() {
    auto pwms_json = OptionsGetJson(OPTIONS_WALK_PWMS);
    int index = 0;
    for (JsonObject item: pwms_json->as<JsonArray>()) {
        String pin_s = item["pin"];
        uint16_t key = RemoteKey::pwm(index++);
        BasicPwm *pwm = nullptr;
        if (pin_s.startsWith(PREFIX_PIN)) {
            auto pwm_io = new PwmIo();
            pwm_io->io_pin = pin_s.substring(strlen(PREFIX_PIN)).toInt();
            pwm_io->setHz(item["hz"].isNull() ? 1000 : item["hz"]);
            pwm_io->attach();
            pwm = pwm_io;
        } else if (pin_s.startsWith(PREFIX_EXTEND)) {
            auto extend = getExtendById(pin_s);
            if (!extend) continue;
            auto extend_pwm = dynamic_cast<BasicPwmExtend *>(extend);
            if (!extend_pwm) continue;
            pwm = extend_pwm->usePwm(extend->id_to_pin[pin_s]);
        }
        if (!pwm) continue;
        pwm->min_pwm = item["min"].isNull() ? 0 : item["min"];
        pwm->max_pwm = item["max"].isNull() ? 255 : item["max"];
        pwm->id = item["id"].as<String>();
        pwm->link_reset = item["reset"].as<bool>();
        pwm->default_value = item["def"].as<int>();
        pwm->reset(true); // 强制回到默认值
        outputs[key] = pwm;
    }
}