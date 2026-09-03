#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "config.h"
#include "pins.h"
#include "plant_config.h"
#include "sensors/sensor_manager.h"
#include "actuators/actuator_manager.h"

static const char *TAG = "MAIN";

static SensorManager   s_sensorManager;
static ActuatorManager s_actuatorManager;

static uint32_t get_millis(void) {
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

static void print_banner(void) {
    printf("\n===============================================================\n");
    printf("       SMART GREENHOUSE IoT - ESP32-WROOM-32 (ESP-IDF)\n");
    printf("            Crop Profile: Basil (Ocimum basilicum)\n");
    printf("===============================================================\n");
    printf("Firmware: %s v%s\n", FIRMWARE_NAME, FIRMWARE_VERSION);
    printf("Target RH: %.0f%% - %.0f%% | Target VPD: %.2f - %.2f kPa\n",
           BASIL_RH_OPTIMAL_MIN, BASIL_RH_OPTIMAL_MAX,
           BASIL_VPD_OPTIMAL_MIN, BASIL_VPD_OPTIMAL_MAX);
    printf("Optimal Temp: %.1f - %.1f C | Soil Moisture: %.0f%% - %.0f%%\n",
           BASIL_GROWTH_TEMP_OPT_MIN, BASIL_GROWTH_TEMP_OPT_MAX,
           BASIL_SOIL_MOISTURE_OPTIMAL_MIN, BASIL_SOIL_MOISTURE_OPTIMAL_MAX);
    printf("===============================================================\n\n");
}

static void print_telemetry_dashboard(uint32_t nowMs) {
    const NormalizedSensorData& s = s_sensorManager.getNormalizedData();
    ActuatorStatus a;
    s_actuatorManager.getStatus(a, nowMs);

    printf("\n------------------- GREENHOUSE TELEMETRY -------------------\n");
    printf("Uptime: %lu s\n", nowMs / 1000);
    
    printf("SOIL:  Moisture: %5.1f %% [%s] | Temp: %5.1f C\n",
           s.soilMoisturePercent,
           (s.soilMoisturePercent < BASIL_SOIL_MOISTURE_IRR_START) ? "DRY - IRR NEEDED" :
           (s.soilMoisturePercent > BASIL_SOIL_MOISTURE_WATERLOG)  ? "WATERLOGGED!" : "OPTIMAL",
           s.soilTemperatureC);
    printf("WATER: Reservoir: %4.1f %% [Safety: %s]\n",
           s.waterLevelPercent,
           s.isWaterReservoirSafe ? "OK" : "CRITICAL LOW - PUMP LOCKED");

    printf("AIR:   Temp: %5.1f C | RH: %5.1f %% | VPD: %4.2f kPa [%s]\n",
           s.ambientTemperatureC, s.ambientHumidityPercent, s.vpdKpa,
           (s.vpdKpa < BASIL_VPD_OPTIMAL_MIN) ? "STAGNANT / MOLD RISK" :
           (s.vpdKpa > BASIL_VPD_OPTIMAL_MAX) ? "EXCESSIVE DRYING" : "OPTIMAL");
    printf("LIGHT: %6.1f Lux [%s]\n",
           s.lightIntensityLux,
           (s.lightIntensityLux >= BASIL_LUX_OPTIMAL_MIN) ? "SUFFICIENT" : "LOW LIGHT");

    printf("ACTUATORS: Pump: [%s] | Fan 1 (Intake): [%s] | Fan 2 (Exhaust): [%s]\n",
           a.pumpActive ? "ON" : "OFF",
           a.air1Active ? "ON" : "OFF",
           a.air2Active ? "ON" : "OFF");
    printf("  Pump Status: %s (Total: ~%.1f mL)\n", a.pumpStatusMessage, a.estimatedWaterDispensedMl);
    printf("------------------------------------------------------------\n");
}

extern "C" void app_main(void) {
    print_banner();

    // Configure status LED
    gpio_config_t led_cfg = {};
    led_cfg.intr_type = GPIO_INTR_DISABLE;
    led_cfg.mode = GPIO_MODE_OUTPUT;
    led_cfg.pin_bit_mask = (1ULL << PIN_STATUS_LED);
    gpio_config(&led_cfg);
    gpio_set_level(PIN_STATUS_LED, 1);

    s_actuatorManager.begin();
    s_sensorManager.begin();

    gpio_set_level(PIN_STATUS_LED, 0);
    ESP_LOGI(TAG, "Automatic greenhouse control loop started.");

    uint32_t lastSensorPollMs  = 0;
    uint32_t lastControlEvalMs = 0;
    uint32_t lastTelemetryMs   = 0;

    while (1) {
        uint32_t nowMs = get_millis();

        if (nowMs - lastSensorPollMs >= SENSOR_READ_INTERVAL_MS) {
            lastSensorPollMs = nowMs;
            s_sensorManager.update(nowMs);
        }

        if (nowMs - lastControlEvalMs >= CONTROL_LOOP_INTERVAL_MS) {
            lastControlEvalMs = nowMs;
            const NormalizedSensorData& sensorData = s_sensorManager.getNormalizedData();
            s_actuatorManager.update(sensorData, nowMs);
        }

        if (nowMs - lastTelemetryMs >= TELEMETRY_PRINT_INTERVAL_MS) {
            lastTelemetryMs = nowMs;
            print_telemetry_dashboard(nowMs);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

