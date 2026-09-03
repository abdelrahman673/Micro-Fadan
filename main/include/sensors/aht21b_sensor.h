#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c_master.h"
#include "config.h"

class AHT21BSensor {
public:
    AHT21BSensor();

    bool begin(i2c_master_bus_handle_t busHandle, uint8_t address = I2C_ADDR_AHT21B);
    bool read(float& temperatureC, float& humidityPercent);
    bool isConnected() const { return _initialized; }

private:
    i2c_master_dev_handle_t _devHandle;
    bool _initialized;
};

