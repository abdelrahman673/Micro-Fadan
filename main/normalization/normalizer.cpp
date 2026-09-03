#include "normalization/normalizer.h"
#include <math.h>

Normalizer::Normalizer() {}

float Normalizer::clampFloat(float val, float minVal, float maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

float Normalizer::normalizeSoilMoisture(uint16_t rawAdc) {
    // Capacitive V1.2 outputs higher voltage in dry air, lower voltage in water
    float dryAdc = (float)SOIL_ADC_DRY_AIR;
    float wetAdc = (float)SOIL_ADC_WET_WATER;

    if (dryAdc <= wetAdc) return 0.0f;

    float percent = ((dryAdc - (float)rawAdc) / (dryAdc - wetAdc)) * 100.0f;
    return clampFloat(percent, 0.0f, 100.0f);
}

float Normalizer::normalizeWaterLevel(uint16_t rawAdc, bool digitalState) {
    if (WATER_SENSOR_TYPE == WaterSensorType::DIGITAL_FLOAT_SWITCH) {
        return digitalState ? 100.0f : 0.0f;
    }

    float emptyAdc = (float)WATER_ADC_EMPTY;
    float fullAdc = (float)WATER_ADC_FULL;

    if (fullAdc <= emptyAdc) return 0.0f;

    float percent = (((float)rawAdc - emptyAdc) / (fullAdc - emptyAdc)) * 100.0f;
    return clampFloat(percent, 0.0f, 100.0f);
}

float Normalizer::calculateVPD(float tempC, float rhPercent) {
    // Tetens equation: VPsat = 0.61078 * exp((17.27 * T) / (T + 237.3)) [kPa]
    if (tempC < -40.0f || tempC > 80.0f) return 0.0f;

    float rhClamped = clampFloat(rhPercent, 0.0f, 100.0f);
    float vpSat = 0.61078f * expf((17.27f * tempC) / (tempC + 237.3f));
    float vpAct = vpSat * (rhClamped / 100.0f);
    float vpd = vpSat - vpAct;

    return clampFloat(vpd, 0.0f, 10.0f);
}

void Normalizer::processRawToNormalized(const RawSensorData& raw, NormalizedSensorData& norm) {
    norm.timestampMs = raw.timestampMs;

    norm.soilMoisturePercent = normalizeSoilMoisture(raw.soilMoistureRawADC);
    norm.soilSensorValid = (raw.soilMoistureRawADC > 100 && raw.soilMoistureRawADC < 4090);

    norm.waterLevelPercent = normalizeWaterLevel(raw.waterLevelRawADC, raw.waterLevelDigitalState);
    norm.isWaterReservoirSafe = (norm.waterLevelPercent >= WATER_RESERVOIR_MIN_SAFETY);
    norm.waterSensorValid = true;

    // DS18B20 returns -127.0°C when disconnected
    if (raw.ds18b20RawTempC > -55.0f && raw.ds18b20RawTempC < 125.0f) {
        norm.soilTemperatureC = raw.ds18b20RawTempC;
        norm.ds18b20Valid = true;
    } else {
        norm.soilTemperatureC = 25.0f;
        norm.ds18b20Valid = false;
    }

    if (raw.aht21bRawTempC > -40.0f && raw.aht21bRawTempC < 85.0f &&
        raw.aht21bRawHumidity >= 0.0f && raw.aht21bRawHumidity <= 100.0f) {
        norm.ambientTemperatureC = raw.aht21bRawTempC;
        norm.ambientHumidityPercent = raw.aht21bRawHumidity;
        norm.aht21bValid = true;
    } else {
        norm.ambientTemperatureC = 25.0f;
        norm.ambientHumidityPercent = 75.0f;
        norm.aht21bValid = false;
    }

    norm.vpdKpa = calculateVPD(norm.ambientTemperatureC, norm.ambientHumidityPercent);

    if (raw.bh1750RawLux >= 0.0f) {
        norm.lightIntensityLux = raw.bh1750RawLux;
        norm.bh1750Valid = true;
    } else {
        norm.lightIntensityLux = 0.0f;
        norm.bh1750Valid = false;
    }
}

