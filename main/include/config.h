#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "pins.h"
#include "plant_config.h"

#define FIRMWARE_NAME                   "SmartGreenhouse-ESP32-IDF"
#define FIRMWARE_VERSION                "1.0.0"
#define TARGET_CROP                     "Basil (Ocimum basilicum)"

// Timing Intervals (ms)
#define SENSOR_READ_INTERVAL_MS         2000
#define CONTROL_LOOP_INTERVAL_MS        1000
#define TELEMETRY_PRINT_INTERVAL_MS     3000

// ADC & Calibration
#define ESP32_ADC_RESOLUTION_BITS       12
#define ESP32_ADC_MAX_VALUE             4095

// Soil Moisture V1.2 (dry air ADC vs fully submerged ADC)
#define SOIL_ADC_DRY_AIR                3150
#define SOIL_ADC_WET_WATER              1350
#define SOIL_OVERSAMPLE_COUNT           16

// Water Level Sensor
enum class WaterSensorType {
    ANALOG_RESISTIVE_STRIP,
    DIGITAL_FLOAT_SWITCH
};
#define WATER_SENSOR_TYPE               WaterSensorType::ANALOG_RESISTIVE_STRIP

// Dry sensor ADC
#define WATER_ADC_EMPTY                 400

// Submerged to max line ADC
#define WATER_ADC_FULL                  2600

#define WATER_OVERSAMPLE_COUNT          16

// Anti-flood continuous run cutoff
#define PUMP_MAX_RUN_TIME_MS            15000

// Mandatory rest time between waterings
#define PUMP_MIN_COOLDOWN_MS            45000

#define PUMP_FLOW_RATE_ML_PER_SEC       25.0f

// Prevents rapid relay chattering
#define AIRFLOW_HYSTERESIS_TEMP_C       1.0f

#define AIRFLOW_HYSTERESIS_RH           3.0f
#define AIRFLOW_MIN_CYCLE_TIME_MS       10000
