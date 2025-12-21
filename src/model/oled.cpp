#include <Adafruit_SSD1306.h>
#include "model/oled.h"
#include "model/options.h"
#include "model/i2c.h"
#include "entity/NMScreenItem.h"
#include <map>

void refreshInfo();

Adafruit_SSD1306 display(128, 32, &Wire, -1);
static const unsigned char PROGMEM logo_bmp[] =
    {3, 255, 255, 192, 15, 255, 255, 240, 31, 255, 255, 248, 63, 255, 255, 252, 127, 255, 255, 254, 126, 31, 248, 126, 252, 31, 248, 63, 252, 31, 248, 63, 252, 0, 0, 63, 252, 0, 0, 63, 252, 30, 120, 63, 252, 30, 120, 63, 252, 30, 120, 63, 255, 254, 127, 255, 255, 254, 127, 255, 255, 254, 127, 255, 255, 254, 127, 255, 255, 254, 127, 255, 255, 254, 127, 255, 252, 30, 120, 63, 252, 30, 120, 63, 252, 30, 120, 63, 252, 0, 0, 63, 252, 0, 0, 63, 252, 31, 248, 63, 252, 31, 248, 63, 126, 31, 248, 126, 127, 255, 255, 254, 63, 255, 255, 252, 31, 255, 255, 248, 15, 255, 255, 240, 3, 255, 255, 192};

const int logo_size = 32 * 32 / 8;

uint8_t logo[logo_size];
bool enable = false;
int height = 0; // 总高度
int y = 0;
std::map<uint16_t, NMScreenItem*> items;

bool isInfo = true;

void TaskScreen(void *parame);

void OledInit(){
    OledReset();
    xTaskCreatePinnedToCore(TaskScreen, "TaskScreen", 2048, nullptr, 1, NULL, 0);
}


void OledReset(){
  JsonDocument json;
  OptionsGetJson(json, OPTIONS_SCREEN);
  if(enable){
      display.clearDisplay();
      display.display();
  }
  enable = false;
  items.clear();
  if(!json["enable"]) return;
  if (!display.begin(SSD1306_SWITCHCAPVCC, json["address"].as<uint8_t>(), false, false)) {
      Serial.println(F("SSD1306 allocation failed"));
      return;
  }
  display.clearDisplay();
  enable = true;

  if(json["logo"].isNull()){
    for (size_t i = 0; i < logo_size; i++) {
        logo[i] = logo_bmp[i];
    }
  }else{
    JsonArray new_logo = json["logo"].as<JsonArray>();
    for (size_t i = 0; i < logo_size; i++) {
        logo[i] = new_logo[i].as<uint8_t>();
    }
  }

  JsonArray modules = json["module"].as<JsonArray>();

  size_t i = 0;
  height = 0;
  for (JsonObject item : modules) {
    items[i] = new NMScreenItem();
    items[i]->setType(item["type"]);
    items[i]->setPrefix(item["prefix"].as<String>());
    items[i]->setSize(item["size"].isNull() ? 1 : item["size"]);
    items[i]->setDelay(item["delay"].isNull() ? 1000 : item["delay"]);
    items[i]->setDisplay(&display);
    height += items[i]->height;
    i++;
  }
  y = 0;
}

void TaskScreen(void *parame){
    while (true){
        if(!enable) {
            vTaskDelay(300 / portTICK_PERIOD_MS); // 延时
            continue;
        }
        if(isInfo){
            refreshInfo();
        }else{
            display.clearDisplay();
            display.drawBitmap(0, 0, logo, 32, 32, 1);
            // 查询当前要显示的模块
            int h = 0; // 累计的高度
            for (const auto &entry : items) {
                NMScreenItem *item = entry.second;

                if(h + y + item->height < 0){
                    continue; // 跳过超出屏幕的模块
                }
                if(h + y > display.height()){
                    break; // 后续模块超出屏幕，停止循环
                }
                item->render(32, h + y);
                h += item->height;
            }
            if(height > display.height() + 4){ // 4 是每项的边距
                for (const auto &entry : items) {
                    if(h + y > display.height()){
                        break; // 后续模块超出屏幕，停止循环
                    }
                    NMScreenItem *item = entry.second;
                    item->render(32, h + y);
                    h += item->height;
                }
                y--;
                if(y < height * -1){
                    y = 0;
                }
            }
            display.display();
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // 延时
    }
}


String texts[] = {"", "", ""};
void OledText(String text){
    texts[0] = texts[1];
    texts[1] = texts[2];
    texts[2] = text;
}

// 启动
void OledSetInfoMode(bool info){
    if(info){
        texts[0] = "";
        texts[1] = "";
        texts[2] = "";
    }
    isInfo = info;
}

void refreshInfo(){
    if(!enable) return;
    display.clearDisplay();
    display.drawBitmap(0, 0, logo_bmp, 32, 32, 1);
    for (size_t i = 0; i < 3; i++)
    {
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); 
        if(i == 0){
            display.setCursor(38,0); 
        }else if(i == 1){
            display.setCursor(38,12); 
        }else{
            display.setCursor(38,24); 
        }
        display.println(texts[i]);
    }
    display.display();
}