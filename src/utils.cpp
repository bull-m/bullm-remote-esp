#include "utils.h"

double mapf(double x, double in_min, double in_max, double out_min, double out_max) {
	if(x>in_max)
		return out_max;
	if(x<in_min)
		return out_min;
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int map_lround(double x, double in_min, double in_max, double out_min, double out_max) {
	return std::lround(mapf(x, in_min, in_max, out_min, out_max));
}

uint16_t merge2byte(uint8_t high, uint8_t low){
	return (high << 8) | low;
}