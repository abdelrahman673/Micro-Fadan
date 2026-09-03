#pragma once

#include <stdint.h>
#include "esp_adc/adc_oneshot.h"
#include "config.h"

class SoilMoistureSensor {
public:
    SoilMoistureSensor(adc_channel_t channel = ADC_CHANNEL_6);

    void setAdcHandle(adc_oneshot_unit_handle_t handle);
    uint16_t readRawADC();

private:
    adc_channel_t _channel;
    adc_oneshot_unit_handle_t _adcHandle;
};

