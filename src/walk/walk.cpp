#include "walk/walk.h"
#include <utils.h>
#include <esp_task_wdt.h>

// 生成数字类型的远程key，和前端代码同步，不允许随便修改，方便用更少的数据(两字节)就定位一个设备
namespace RemoteKey {
    // 自定义输出
    int digital(uint16_t index) {
        return 10000 + index; // 10000 ~ 11999
    }

    // 自定义pwm
    int pwm(uint16_t index) {
        return 12000 + index; // 12000 ~ 13999
    }

    // 自定义舵机
    int servo(uint16_t index) {
        return 14000 + index; // 14000 ~ 15999
    }

    // 自定义组合
    int group(uint16_t index) {
        return 16000 + index; // 16000 ~ 17999
    }

    // 扩展里面定义的固定设备
    int extendDrive(uint16_t index) {
        return 18000 + index; // 18000 ~ 19999
    }
}

namespace PinId {
    String extend(const String &extend_id, uint8_t pin) {
        return PREFIX_EXTEND + extend_id + "-" + String(pin);
    }

    String pin(uint8_t pin) {
        return PREFIX_PIN + String(pin);
    }
}


// 生成设备的完整id
namespace DeviceId {
    // 一个组合会使用多个设备，使用下标生成id
    String extendGroup(const String &extend_id, uint8_t index) {
        return PREFIX_GROUP + extend_id + "-" + index;
    }

    // 舵机、pwm、电平、引脚都要pin字段且都是唯一的，所以用pin生成唯一id
    String extendServo(const String &extend_id, uint8_t pin) {
        return PREFIX_SERVO + extend_id + "-" + pin;
    }

    String extendPwm(const String &extend_id, uint8_t pin) {
        return PREFIX_PWM + extend_id + "-" + pin;
    }

    String extendDigital(const String &extend_id, uint8_t pin) {
        return PREFIX_DIGITAL + extend_id + "-" + pin;
    }
}

std::map<uint16_t, BasicOutput *> outputs;

// 根据id获取PWM对象
BasicOutput *getOutputById(const String &id) {
    for (const auto &entry: outputs) {
        if (entry.second->id == id) {
            return entry.second;
        }
    }
    return nullptr;
}

// 添加超时检测相关变量
static unsigned long last_time = 0;

// 卸载所有
void detach_all() {
    for (auto &entry: outputs) {
        auto &item = entry.second;
        if (item != nullptr) {
            item->detach();
            delete item;
            item = nullptr;
        }
    }
    outputs.clear();
    ExtendDetach();
}

void WalkInit() {
    detach_all(); // 先按顺序清空所有对象，防止指针错误
    InitExtend();
    InitPwm();
    InitDigital();
    InitServo();
    InitGroup();
    last_time = millis(); // 初始化最后接收时间
}

// 重置状态
void WalkReset() {
    ExtendReset();
    for (auto &entry: outputs) {
        if (entry.second != nullptr) {
            entry.second->reset(false);
        }
    }
}


// 检查UDP超时 还没用上
void WalkCheckTimeout() {
    // 每秒检查一次超时
    if (millis() - last_time > 1000) {
//        for (const auto &entry: groups) {
//            entry->checkTimeout();
//        }
        last_time = millis(); // 更新最后检查时间
    }
}

// 处理客户端请求（json）
JsonDocument *WalkHandle(JsonDocument &data) {
    String mode = data["mode"];
    auto *req = new JsonDocument();
    auto &json = *req;
    JsonObject obj = json["data"].to<JsonObject>();
    if (mode == "device-state") {
        for (const auto &entry: outputs) {
            auto device = entry.second;
            if (device != nullptr) {
                obj[device->id] = device->read();
            }
        }
    }
    return req;
}

// 处理运动数据 （字节）
void WalkHandle(uint8_t *data, size_t len) {
    uint8_t mode = data[0];
    if (mode < 0x10 || mode >= 0x20) {
        return;
    }
    for (int i = 1; i < len;) {
        esp_task_wdt_reset(); // 喂食狗
        switch (mode) {
            // (由于1字节和2字节的数据使用的多，所有给一个独立的控制指令，省一字节)
            // 一字节数据
            case CONTROL_BYTE1: {
                uint8_t high = data[i++];
                uint16_t key = merge2byte(high, data[i++]);
                uint8_t value = data[i++]; // 读取数据
                if (!outputs[key]) continue; // 找不到设备
                outputs[key]->write(value);
                break;
            }
                // 两字节数据
            case CONTROL_BYTE2: {
                uint8_t high = data[i++];
                uint16_t key = merge2byte(high, data[i++]);
                int values[2] = {data[i++], data[i++]};
                if (!outputs[key]) continue; // 找不到设备
                outputs[key]->write(values, 2);
                break;
            }
                // 多字节数据
            case CONTROL_BYTES: {
                uint8_t high = data[i++];
                uint16_t key = merge2byte(high, data[i++]);
                uint8_t length = data[i++];
                int values[length];
                for (int j = 0; j < length; j++) {
                    values[j] = data[i++];
                }
                if (!outputs[key]) continue; // 找不到设备
                outputs[key]->write(values, length);
                break;
            }

                // 没用上
            case CONTROL_MAINTAIN: {
                uint8_t high = data[i++];
                uint16_t key = merge2byte(high, data[i++]);
                if (!outputs[key]) continue;
                outputs[key]->last_update_time = millis(); // 刷新最后更新时间
                break;
            }
            default:
                break;
        }
    }
}