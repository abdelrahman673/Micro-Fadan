#include "sensors/bh1750_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

BH1750LightSensor::BH1750LightSensor() : _devHandle(nullptr), _initialized(false) {}

bool BH1750LightSensor::begin(i2c_master_bus_handle_t busHandle, uint8_t address) {
    if (!busHandle) return false;

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = address;
    dev_cfg.scl_speed_hz = 100000;

    if (i2c_master_bus_add_device(busHandle, &dev_cfg, &_devHandle) != ESP_OK) {
        _initialized = false;
        return false;
    }

    // Power on command
    uint8_t powerOn = 0x01;
    i2c_master_transmit(_devHandle, &powerOn, 1, 100);

    // Continuous H-Resolution Mode (1 lx resolution)
    uint8_t mode = 0x10;
    i2c_master_transmit(_devHandle, &mode, 1, 100);
    vTaskDelay(pdMS_TO_TICKS(120));

    _initialized = true;
    return true;
}

float BH1750LightSensor::readLux() {
    if (!_initialized || !_devHandle) return -1.0f;

    uint8_t data[2] = {0};
    if (i2c_master_receive(_devHandle, data, 2, 100) != ESP_OK) {
        return -1.0f;
    }

    uint16_t raw = ((uint16_t)data[0] << 8) | data[1];
    return (float)raw / 1.2f;
}

