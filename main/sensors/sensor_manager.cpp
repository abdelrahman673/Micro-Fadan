#include "sensors/sensor_manager.h"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "SENSOR";

SensorManager::SensorManager() : _adcHandle(nullptr), _i2cBusHandle(nullptr) {
    memset(&_raw, 0, sizeof(_raw));
    memset(&_normalized, 0, sizeof(_normalized));
}

bool SensorManager::begin() {
    // 1. Initialize ADC1 for Soil Moisture (CH6 / GPIO34) and Water Level (CH7 / GPIO35)
    adc_oneshot_unit_init_cfg_t adc_init_cfg = {};
    adc_init_cfg.unit_id = ADC_UNIT_1;
    adc_oneshot_new_unit(&adc_init_cfg, &_adcHandle);

    adc_oneshot_chan_cfg_t chan_cfg = {};
    chan_cfg.atten = ADC_ATTEN_DB_12;
    chan_cfg.bitwidth = ADC_BITWIDTH_12;

    adc_oneshot_config_channel(_adcHandle, ADC_CHANNEL_6, &chan_cfg);
    adc_oneshot_config_channel(_adcHandle, ADC_CHANNEL_7, &chan_cfg);

    _soilSensor.setAdcHandle(_adcHandle);
    _waterSensor.setAdcHandle(_adcHandle);
    _waterSensor.begin();

    // 2. Initialize DS18B20 on 1-Wire
    bool dsOk = _ds18b20.begin();
    if (!dsOk) {
        ESP_LOGW(TAG, "DS18B20 not detected (check 4.7k pull-up resistor on GPIO %d)", PIN_ONEWIRE_DS18B20);
    }

    // 3. Initialize I2C Bus for AHT21B & BH1750
    i2c_master_bus_config_t i2c_bus_config = {};
    i2c_bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_bus_config.i2c_port = I2C_NUM_0;
    i2c_bus_config.scl_io_num = PIN_I2C_SCL;
    i2c_bus_config.sda_io_num = PIN_I2C_SDA;
    i2c_bus_config.glitch_ignore_cnt = 7;
    i2c_bus_config.flags.enable_internal_pullup = true;

    if (i2c_new_master_bus(&i2c_bus_config, &_i2cBusHandle) == ESP_OK) {
        if (!_aht21b.begin(_i2cBusHandle, I2C_ADDR_AHT21B)) {
            ESP_LOGW(TAG, "AHT21B not detected at 0x%02X", I2C_ADDR_AHT21B);
        }
        if (!_bh1750.begin(_i2cBusHandle, I2C_ADDR_BH1750)) {
            ESP_LOGW(TAG, "BH1750 not detected at 0x%02X", I2C_ADDR_BH1750);
        }
    } else {
        ESP_LOGE(TAG, "Failed to initialize I2C master bus");
    }

    update(0);
    return true;
}

void SensorManager::update(uint32_t currentMillis) {
    _raw.timestampMs = currentMillis;

    _raw.soilMoistureRawADC = _soilSensor.readRawADC();
    _raw.waterLevelRawADC = _waterSensor.readRawADC();
    _raw.waterLevelDigitalState = _waterSensor.readDigitalState();
    _raw.ds18b20RawTempC = _ds18b20.readTemperatureC();

    float ahtTemp = 0.0f, ahtHum = 0.0f;
    if (_aht21b.read(ahtTemp, ahtHum)) {
        _raw.aht21bRawTempC = ahtTemp;
        _raw.aht21bRawHumidity = ahtHum;
    } else {
        _raw.aht21bRawTempC = -999.0f;
        _raw.aht21bRawHumidity = -999.0f;
    }

    _raw.bh1750RawLux = _bh1750.readLux();

    Normalizer::processRawToNormalized(_raw, _normalized);
}

void SensorManager::printDiagnosticReport() const {
    ESP_LOGI(TAG, "========== SENSOR DIAGNOSTIC REPORT ==========");
    ESP_LOGI(TAG, "Soil Moisture Raw: %u -> Normalized: %.1f %%",
             _raw.soilMoistureRawADC, _normalized.soilMoisturePercent);
    ESP_LOGI(TAG, "DS18B20 Temp Raw: %.2f C -> Valid: %s",
             _raw.ds18b20RawTempC, _normalized.ds18b20Valid ? "YES" : "NO");
    ESP_LOGI(TAG, "Water Level Raw: %u -> Level: %.1f %% (Safe: %s)",
             _raw.waterLevelRawADC, _normalized.waterLevelPercent,
             _normalized.isWaterReservoirSafe ? "YES" : "NO - LOW WATER");
    ESP_LOGI(TAG, "AHT21B Air Temp: %.2f C | RH: %.2f %% -> VPD: %.2f kPa",
             _normalized.ambientTemperatureC, _normalized.ambientHumidityPercent, _normalized.vpdKpa);
    ESP_LOGI(TAG, "BH1750 Light: %.1f Lux", _normalized.lightIntensityLux);
    ESP_LOGI(TAG, "==============================================");
}

