#include "walk/walk.h"
#include "walk/group/GroupMotor.h"

// 组合初始化的辅助函数
void GroupItemInit(JsonObject *_item, int key) {
    JsonObject item = *_item;
    String type = item["type"];
    auto group = new GroupMotor();
    if (type == GROUP_TYPE_2D1PWM) {
        group->attach(
                getOutputById(item["forward"]),
                getOutputById(item["back"]),
                getOutputById(item["pwm"]));
    } else if (type == GROUP_TYPE_2EN2PWM) {
        group->attach(
                getOutputById(item["forward"]),
                getOutputById(item["back"]),
                getOutputById(item["forward_pwm"]),
                getOutputById(item["back_pwm"]));
    } else if (type == GROUP_TYPE_2PWM) {
        group->attach(getOutputById(item["forward"]), getOutputById(item["back"]));
    } else if (type == GROUP_TYPE_1PWM) {
        group->attach(getOutputById(item["pwm"]));
    } else {
        delete group;
        return;
    }
    group->id = item["id"].as<String>();
    group->min_speed = item["min"].isNull() ? 0 : item["min"];
    group->max_speed = item["max"].isNull() ? 255 : item["max"];
    group->reversal = item["reversal"].as<bool>();
    group->auto_brake = item["auto_brake"].as<bool>();
    group->last_update_time = millis(); // 最后的更新时间
    group->reset();
    outputs[key] = group;
}

// 组合初始化
void InitGroup() {
    int index = 0;
    auto groups_json = OptionsGetJson(OPTIONS_WALK_GROUPS);
    for (JsonObject item: groups_json->as<JsonArray>()) {
        GroupItemInit(&item, RemoteKey::group(index++));
    }
}