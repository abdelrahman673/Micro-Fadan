#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c_master.h"
#include "config.h"

class BH1750LightSensor {
public:
    BH1750LightSensor();

    bool begin(i2c_master_bus_handle_t busHandle, uint8_t address = I2C_ADDR_BH1750);
    float readLux();
    bool isConnected() const { return _initialized; }

private:
    i2c_master_dev_handle_t _devHandle;
    bool _initialized;
};

