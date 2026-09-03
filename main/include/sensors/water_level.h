#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "config.h"

class WaterLevelSensor {
public:
    WaterLevelSensor(adc_channel_t adcChannel = ADC_CHANNEL_7,
                     gpio_num_t digitalPin = PIN_WATER_LEVEL_DIGITAL,
                     WaterSensorType type = WATER_SENSOR_TYPE);

    void setAdcHandle(adc_oneshot_unit_handle_t handle);
    void begin();
    uint16_t readRawADC();
    bool readDigitalState();

private:
    adc_channel_t _adcChannel;
    gpio_num_t    _digitalPin;
    WaterSensorType _type;
    adc_oneshot_unit_handle_t _adcHandle;
};

