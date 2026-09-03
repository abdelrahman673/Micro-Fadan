#pragma once

enum class PlantGrowthStage {
    GERMINATION,
    VEGETATIVE_GROWTH
};

#define CURRENT_GROWTH_STAGE            PlantGrowthStage::VEGETATIVE_GROWTH

// Temperature (°C)
#define BASIL_GERM_TEMP_EXTREME_LOW     12.0f
#define BASIL_GERM_TEMP_OPT_MIN         24.0f
#define BASIL_GERM_TEMP_OPT_MAX         29.0f
#define BASIL_GERM_TEMP_EXTREME_HIGH    30.0f

#define BASIL_GROWTH_TEMP_EXTREME_LOW   15.0f
#define BASIL_GROWTH_TEMP_OPT_MIN       26.0f
#define BASIL_GROWTH_TEMP_OPT_MAX       32.0f
#define BASIL_GROWTH_TEMP_EXTREME_HIGH  35.0f

// Optimal RH lower limit
#define BASIL_RH_OPTIMAL_MIN            70.0f

// Optimal RH upper limit
#define BASIL_RH_OPTIMAL_MAX            85.0f

// Triggers ventilation to prevent downy mildew/mold
#define BASIL_RH_CRITICAL_HIGH          85.0f

// Target VPD lower bound (kPa)
#define BASIL_VPD_OPTIMAL_MIN           0.65f

// Target VPD upper bound (kPa)
#define BASIL_VPD_OPTIMAL_MAX           1.00f

// Low VPD (stagnant/humid) triggers airflow
#define BASIL_VPD_CRITICAL_LOW          0.60f

#define BASIL_VPD_CRITICAL_HIGH         1.20f

// Start pump below 40%
#define BASIL_SOIL_MOISTURE_IRR_START   40.0f

// Stop pump at 68%
#define BASIL_SOIL_MOISTURE_IRR_STOP    68.0f

#define BASIL_SOIL_MOISTURE_OPTIMAL_MIN 50.0f
#define BASIL_SOIL_MOISTURE_OPTIMAL_MAX 70.0f
#define BASIL_SOIL_MOISTURE_WATERLOG    85.0f

// Lockout pump below 15% to prevent dry-running
#define WATER_RESERVOIR_MIN_SAFETY      15.0f

#define WATER_RESERVOIR_LOW_WARNING     25.0f

// Light Intensity (Lux)
#define BASIL_LUX_MIN_GROWTH            5000.0f
#define BASIL_LUX_OPTIMAL_MIN           10000.0f
#define BASIL_LUX_OPTIMAL_MAX           30000.0f
#define BASIL_LUX_DIRECT_SUN_MAX        50000.0f
