#pragma once

#include <stdint.h>
#include <stdbool.h>

struct RawSensorData {
    uint16_t soilMoistureRawADC;
    uint16_t waterLevelRawADC;
    bool     waterLevelDigitalState;
    float    ds18b20RawTempC;
    float    aht21bRawTempC;
    float    aht21bRawHumidity;
    float    bh1750RawLux;
    uint32_t timestampMs;
};

struct NormalizedSensorData {
    // 0.0% to 100.0%
    float soilMoisturePercent;
    float soilTemperatureC;

    // 0.0% to 100.0%
    float waterLevelPercent;
    bool  isWaterReservoirSafe;

    float ambientTemperatureC;
    float ambientHumidityPercent;

    // Vapor Pressure Deficit in kPa
    float vpdKpa;

    float lightIntensityLux;

    bool  soilSensorValid;
    bool  waterSensorValid;
    bool  ds18b20Valid;
    bool  aht21bValid;
    bool  bh1750Valid;

    uint32_t timestampMs;
};
