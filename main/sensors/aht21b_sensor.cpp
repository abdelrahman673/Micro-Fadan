#include "sensors/aht21b_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

AHT21BSensor::AHT21BSensor() : _devHandle(nullptr), _initialized(false) {}

bool AHT21BSensor::begin(i2c_master_bus_handle_t busHandle, uint8_t address) {
    if (!busHandle) return false;

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = address;
    dev_cfg.scl_speed_hz = 100000;

    if (i2c_master_bus_add_device(busHandle, &dev_cfg, &_devHandle) != ESP_OK) {
        _initialized = false;
        return false;
    }

    // Soft reset
    uint8_t resetCmd = 0xBA;
    i2c_master_transmit(_devHandle, &resetCmd, 1, 100);
    vTaskDelay(pdMS_TO_TICKS(20));

    // Initialization command
    uint8_t initCmd[3] = {0xBE, 0x08, 0x00};
    i2c_master_transmit(_devHandle, initCmd, 3, 100);
    vTaskDelay(pdMS_TO_TICKS(10));

    _initialized = true;
    return true;
}

bool AHT21BSensor::read(float& temperatureC, float& humidityPercent) {
    if (!_initialized || !_devHandle) return false;

    // Trigger measurement
    uint8_t triggerCmd[3] = {0xAC, 0x33, 0x00};
    if (i2c_master_transmit(_devHandle, triggerCmd, 3, 100) != ESP_OK) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(80));

    uint8_t data[6] = {0};
    if (i2c_master_receive(_devHandle, data, 6, 100) != ESP_OK) {
        return false;
    }

    // Check busy bit (bit 7 of status byte)
    if (data[0] & 0x80) return false;

    uint32_t rawHumidity = (((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | ((uint32_t)data[3] >> 4));
    uint32_t rawTemp = ((((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | (uint32_t)data[5]);

    humidityPercent = ((float)rawHumidity / 1048576.0f) * 100.0f;
    temperatureC    = (((float)rawTemp / 1048576.0f) * 200.0f) - 50.0f;

    return true;
}

