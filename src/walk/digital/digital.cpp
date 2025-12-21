#include "walk/walk.h"
#include "walk/digital/DigitalIO.h"
#include "walk/digital/DigitalByExtend.h"

void InitDigital() {
    auto outputs_json = OptionsGetJson(OPTIONS_WALK_DIGITALS);
    int index = 0;
    for (JsonObject item: outputs_json->as<JsonArray>()) {
        String pin_s = item["pin"];
        uint16_t key = RemoteKey::digital(index++);
        BasicDigital *digital = nullptr;
        if (pin_s.startsWith(PREFIX_PIN)) {
            auto *digital_io = new DigitalIO();
            digital_io->io_pin = pin_s.substring(strlen(PREFIX_PIN)).toInt();
            digital_io->attach();
            digital = digital_io;
        } else if (pin_s.startsWith(PREFIX_EXTEND)) {
            auto *extend = getExtendById(pin_s);
            if (!extend) continue;
            auto *extend_digital = dynamic_cast<BasicDigitalExtend *>(extend);
            if (!extend_digital) continue;
            digital = extend_digital->useDigital(extend->id_to_pin[pin_s]);
        }
        if (!digital) continue;
        digital->id = item["id"].as<String>();
        digital->default_value = item["def"].isNull() ? 0 : item["def"].as<int>();
        digital->link_reset = item["reset"].as<bool>();
        digital->reset(true); // 强制回到默认值
        outputs[key] = digital;
    }
}
