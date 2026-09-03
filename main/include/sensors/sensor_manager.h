#pragma once

#include "sensors/sensor_types.h"
#include "sensors/soil_moisture.h"
#include "sensors/ds18b20_sensor.h"
#include "sensors/water_level.h"
#include "sensors/aht21b_sensor.h"
#include "sensors/bh1750_sensor.h"
#include "normalization/normalizer.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/i2c_master.h"

class SensorManager {
public:
    SensorManager();

    bool begin();
    void update(uint32_t currentMillis);

    const RawSensorData& getRawData() const { return _raw; }
    const NormalizedSensorData& getNormalizedData() const { return _normalized; }

    void printDiagnosticReport() const;

private:
    SoilMoistureSensor _soilSensor;
    DS18B20Sensor      _ds18b20;
    WaterLevelSensor   _waterSensor;
    AHT21BSensor       _aht21b;
    BH1750LightSensor  _bh1750;

    RawSensorData        _raw;
    NormalizedSensorData _normalized;

    adc_oneshot_unit_handle_t _adcHandle;
    i2c_master_bus_handle_t   _i2cBusHandle;
};

