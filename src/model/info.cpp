#include "model/info.h"



JsonDocument * InfoHandle(JsonDocument &data){
    String mode = data["mode"];
    auto *req = new JsonDocument();
    auto &json = *req;
    json["chipModel"] = ESP.getChipModel();
    json["sdkVersion"] = ESP.getSdkVersion();
    json["sketchFree"] = ESP.getFreeSketchSpace(); // 主程序分区剩余空间
    json["sketchSize"] = ESP.getSketchSize(); // 程序大小
    json["psramSize"] = ESP.getPsramSize(); // 外部 PSRAM 大小
    json["psramFree"] = ESP.getFreePsram(); // 外部 PSRAM 剩余空间
    json["flashSize"]  = ESP.getFlashChipSize(); // Flash 总大小
    json["heapSize"] = ESP.getHeapSize(); // 内部堆大小
    json["heapFree"]  = ESP.getFreeHeap(); // 堆剩余空间

    return req;
}