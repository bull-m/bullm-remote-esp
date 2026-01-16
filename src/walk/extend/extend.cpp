#include "walk/walk.h"
#include "walk/extend/I2CExtendPCA9685.h"
#include "walk/extend/I2CExtendPCA9555.h"
#include "model/i2c.h"
#include "link/link.h"


std::map<String, BasicPINExtend *> extends;

// 根据id获取扩展对象
BasicPINExtend *getExtendById(const String &id) {
    // 不是干净的扩展id，是指定扩展引脚的id
    if (id.startsWith(PREFIX_EXTEND)) {
        String id2 = id.substring(strlen(PREFIX_EXTEND), id.indexOf("-"));
        return extends[id2];
    }
    return extends[id];
}


// 卸载所有
void ExtendDetach() {
    for (auto &entry: extends) {
        auto item = entry.second;
        if (item != nullptr) {
            item->detach();
            delete item;
            entry.second = nullptr;
        }
    }
    extends.clear();
}

// 重置状态
void ExtendReset() {
    for (const auto &entry: extends) {
        auto item = entry.second;
        if (item != nullptr) {
            item->reset();
        }
    }
}


void child_digitals(JsonArray *_digitals, BasicDigitalExtend *extend, const String &id, int *index) {
    JsonArray digitals = *_digitals;
    for (JsonObject item: digitals) {
        auto pin = item["pin"].as<uint8_t>();
        auto *digital = extend->useDigital(pin);
        digital->id = DeviceId::extendDigital(id, pin);
        digital->default_value = !item["def"].isNull() && item["def"];
        digital->link_reset = item["reset"].as<bool>();
        digital->reset(true); // 强制回到默认值
        uint16_t key = RemoteKey::extendDrive(*index); // 扩展的key
        *index += 1;
        outputs[key] = digital;
    }
}

void child_pwms(JsonArray *_pwms, BasicPwmExtend *extend, const String &id, int *index) {
    JsonArray pwms = *_pwms;
    for (JsonObject item: pwms) {
        auto pin = item["pin"].as<uint8_t>();
        auto *pwm = extend->usePwm(pin);
        // 生成id,与前端对应，不允许顺便修改
        pwm->id = DeviceId::extendPwm(id, pin);
        pwm->default_value = item["def"].isNull() ? 0 : item["def"];
        pwm->min_pwm = item["min"].isNull() ? 0 : item["min"];
        pwm->max_pwm = item["max"].isNull() ? 255 : item["max"];
        pwm->link_reset = item["reset"].as<bool>();
        pwm->reset(true); // 强制回到默认值
        uint16_t key = RemoteKey::extendDrive(*index); // 扩展的key
        *index += 1;
        outputs[key] = pwm;
    }
}

void child_servos(JsonArray *_servos, BasicServoExtend *extend, const String &id, int *index) {
    JsonArray servos = *_servos;
    for (JsonObject item: servos) {
        auto pin = item["pin"].as<uint8_t>();
        auto *servo = extend->useServo(pin);
        servo->id = DeviceId::extendServo(id, pin);
        servo->min_angle = item["min"].isNull() ? 0 : item["min"];
        servo->max_angle = item["max"].isNull() ? 180 : item["max"];
        servo->min_pulse = item["pulse_min"].isNull() ? SERVO_uS_LOW_DEFAULT : item["pulse_min"];
        servo->max_pulse = item["pulse_max"].isNull() ? SERVO_uS_HIGH_DEFAULT : item["pulse_max"];
        servo->default_value = !item["def"].isNull() && item["def"];
        servo->link_reset = item["reset"].as<bool>();
        servo->reset(true); // 强制回到默认值
        uint16_t key = RemoteKey::extendDrive(*index); // 扩展的key
        *index += 1;
        outputs[key] = servo;
    }
}

void child_groups(JsonArray *_groups, const String &id, int *index) {
    JsonArray groups = *_groups;
    int g_index = 0;
    for (JsonObject item2: groups) {
        item2["id"] = DeviceId::extendGroup(id, g_index); // Group 是最后一个层级, 这id对于小车端没有用
        g_index++;
        GroupItemInit(&item2, RemoteKey::extendDrive(*index));
        *index += 1;
    }
}

void child_pins(JsonArray *_pins, BasicPINExtend *extend) {
    JsonArray pins = *_pins;
    for (JsonObject item: pins) {
        auto pin = item["pin"].as<uint8_t>();
        String pin_id = PinId::extend(extend->id, pin);
        extend->id_to_pin[pin_id] = pin;
    }
}


// 扩展初始化
void InitExtend() {
    auto extends_json = OptionsGetJson(OPTIONS_WALK_EXTENDS);
    int index = 0; // 用于生成id
    for (JsonObject v: extends_json->as<JsonArray>()) {
        String chip = v["chip"];

        // 加载顺序不允许改变 = 电平 > PWM > Servo > 组合
        if(chip == "pca9685"){
            auto *pca9685 = new I2CExtendPCA9685();
            auto address = v["address"].as<uint8_t>();
            pca9685->hz = v["hz"].as<int>();
            pca9685->address = address;
            pca9685->id = v["id"].as<String>();
            pca9685->attach();
            pca9685->reset();
            extends[pca9685->id] = pca9685;

            // 初始化可自定义的pin
            auto pins = v["pins"].as<JsonArray>();
            child_pins(&pins, pca9685);

            // 初始化自带的电平输出
            auto digitals = v["digitals"].as<JsonArray>();
            child_digitals(&digitals, pca9685, pca9685->id, &index);

            // 初始化自带的pwm输出
            auto pwms = v["pwms"].as<JsonArray>();
            child_pwms(&pwms, pca9685, pca9685->id, &index);

            // 初始化自带的舵机
            auto servos = v["servos"].as<JsonArray>();
            child_servos(&servos, pca9685, pca9685->id, &index);

            auto groups = v["groups"].as<JsonArray>();
            child_groups(&groups, pca9685->id, &index);
        }
        if (chip == "pca9555") {
            auto pca9555 = new I2CExtendPCA9555();
            auto address = v["address"].as<uint8_t>();
            pca9555->hz = v["hz"].as<int>();
            pca9555->address = address;
            pca9555->id = v["id"].as<String>();
            pca9555->attach();
            pca9555->reset();
            extends[pca9555->id] = pca9555;

            // 初始化自带的电平输出
            auto pins = v["pins"].as<JsonArray>();
            child_pins(&pins, pca9555);
            // 初始化自带的输出
            auto digitals = v["digitals"].as<JsonArray>();
            child_digitals(&digitals, pca9555, pca9555->id, &index);
            // TODO 电平输入
        }
    }
}