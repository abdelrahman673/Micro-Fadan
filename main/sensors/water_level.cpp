#include "sensors/water_level.h"
#include "esp_rom_sys.h"

WaterLevelSensor::WaterLevelSensor(adc_channel_t adcChannel, gpio_num_t digitalPin, WaterSensorType type)
    : _adcChannel(adcChannel),
      _digitalPin(digitalPin),
      _type(type),
      _adcHandle(nullptr) {}

void WaterLevelSensor::setAdcHandle(adc_oneshot_unit_handle_t handle) {
    _adcHandle = handle;
}

void WaterLevelSensor::begin() {
    if (_type == WaterSensorType::DIGITAL_FLOAT_SWITCH) {
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pin_bit_mask = (1ULL << _digitalPin);
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpio_config(&io_conf);
    }
}

uint16_t WaterLevelSensor::readRawADC() {
    if (_type != WaterSensorType::ANALOG_RESISTIVE_STRIP || !_adcHandle) return 0;

    uint32_t sum = 0;
    int raw = 0;
    for (uint8_t i = 0; i < WATER_OVERSAMPLE_COUNT; i++) {
        if (adc_oneshot_read(_adcHandle, _adcChannel, &raw) == ESP_OK) {
            sum += raw;
        }
        esp_rom_delay_us(250);
    }
    return (uint16_t)(sum / WATER_OVERSAMPLE_COUNT);
}

bool WaterLevelSensor::readDigitalState() {
    if (_type == WaterSensorType::DIGITAL_FLOAT_SWITCH) {
        return (gpio_get_level(_digitalPin) == 0);
    }
    return (readRawADC() > (WATER_ADC_EMPTY + 50));
}

