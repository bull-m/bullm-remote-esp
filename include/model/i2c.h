#include <Wire.h>
#include <ArduinoJson.h>
#include <map>
#include <AsyncWebSocket.h>

void I2cInit();

JsonDocument * I2cHandle(JsonDocument &data);
bool I2cRead(uint8_t address, uint8_t *buffer, size_t len, bool stop = true);
bool I2cWrite(uint8_t address, const uint8_t *buffer, size_t len, bool stop = true);
bool I2cWriteThenRead(uint8_t address, const uint8_t *write_buffer,
                      size_t write_len, uint8_t *read_buffer,
                      size_t read_len, bool stop = false);