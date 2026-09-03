#pragma once

#include "sensors/sensor_types.h"
#include "config.h"

class Normalizer {
public:
    Normalizer();

    static float normalizeSoilMoisture(uint16_t rawAdc);
    static float normalizeWaterLevel(uint16_t rawAdc, bool digitalState);

    // Arden Buck / Tetens equation: returns VPD in kPa
    static float calculateVPD(float tempC, float rhPercent);

    static void processRawToNormalized(const RawSensorData& raw, NormalizedSensorData& normalized);

private:
    static float clampFloat(float val, float minVal, float maxVal);
};

