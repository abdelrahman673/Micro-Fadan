#include "sensors/soil_moisture.h"
#include "esp_rom_sys.h"

SoilMoistureSensor::SoilMoistureSensor(adc_channel_t channel)
    : _channel(channel), _adcHandle(nullptr) {}

void SoilMoistureSensor::setAdcHandle(adc_oneshot_unit_handle_t handle) {
    _adcHandle = handle;
}

uint16_t SoilMoistureSensor::readRawADC() {
    if (!_adcHandle) return 0;

    uint32_t sum = 0;
    int raw = 0;
    for (uint8_t i = 0; i < SOIL_OVERSAMPLE_COUNT; i++) {
        if (adc_oneshot_read(_adcHandle, _channel, &raw) == ESP_OK) {
            sum += raw;
        }
        esp_rom_delay_us(250);
    }
    return (uint16_t)(sum / SOIL_OVERSAMPLE_COUNT);
}

