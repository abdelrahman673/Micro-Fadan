#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "config.h"

class DS18B20Sensor {
public:
    DS18B20Sensor(gpio_num_t pin = PIN_ONEWIRE_DS18B20);

    bool begin();
    float readTemperatureC();
    bool isConnected() const { return _deviceFound; }

private:
    bool reset();
    void writeBit(uint8_t bit);
    uint8_t readBit();
    void writeByte(uint8_t byte);
    uint8_t readByte();

    gpio_num_t _pin;
    bool _deviceFound;
};

