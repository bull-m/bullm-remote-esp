#include <ArduinoJson.h>

double mapf(double x, double in_min, double in_max, double out_min, double out_max);

// 映射 四舍五入
int map_lround(double x, double in_min, double in_max, double out_min, double out_max);

uint16_t merge2byte(uint8_t high, uint8_t low);