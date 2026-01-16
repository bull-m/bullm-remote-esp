#include "model/error.h"
#include "link/link.h"
#include <list>

std::list<String> errors;

void ErrorSendToClient(){
    if(!WsIsLink()) return;
    for (const auto &item: errors){
        JsonDocument json;
        json["type"] = "msg";
        json["msg"] = item;
        json["msg_type"] = "error"; // 错误信息
        json["msg_delay"] = 10000; // 10秒关闭
        WsAutoSend(json);
        json.clear();
    }
    errors.clear();
}

void ErrorAdd(String str){
    Serial.println(str);
    errors.push_back(str);
    ErrorSendToClient();
}