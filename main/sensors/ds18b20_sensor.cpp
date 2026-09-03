#include "sensors/ds18b20_sensor.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static portMUX_TYPE s_mux = portMUX_INITIALIZER_UNLOCKED;

DS18B20Sensor::DS18B20Sensor(gpio_num_t pin)
    : _pin(pin), _deviceFound(false) {}

bool DS18B20Sensor::begin() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // Open-drain output with pull-up
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
    io_conf.pin_bit_mask = (1ULL << _pin);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(_pin, 1);
    _deviceFound = reset();
    return _deviceFound;
}

bool DS18B20Sensor::reset() {
    portENTER_CRITICAL(&s_mux);
    gpio_set_level(_pin, 0);
    esp_rom_delay_us(480);
    gpio_set_level(_pin, 1);
    esp_rom_delay_us(70);
    int presence = gpio_get_level(_pin);
    portEXIT_CRITICAL(&s_mux);

    esp_rom_delay_us(410);
    return (presence == 0);
}

void DS18B20Sensor::writeBit(uint8_t bit) {
    portENTER_CRITICAL(&s_mux);
    gpio_set_level(_pin, 0);
    if (bit) {
        esp_rom_delay_us(6);
        gpio_set_level(_pin, 1);
        esp_rom_delay_us(64);
    } else {
        esp_rom_delay_us(60);
        gpio_set_level(_pin, 1);
        esp_rom_delay_us(10);
    }
    portEXIT_CRITICAL(&s_mux);
}

uint8_t DS18B20Sensor::readBit() {
    portENTER_CRITICAL(&s_mux);
    gpio_set_level(_pin, 0);
    esp_rom_delay_us(3);
    gpio_set_level(_pin, 1);
    esp_rom_delay_us(10);
    uint8_t bit = gpio_get_level(_pin);
    portEXIT_CRITICAL(&s_mux);

    esp_rom_delay_us(55);
    return bit;
}

void DS18B20Sensor::writeByte(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        writeBit(byte & 0x01);
        byte >>= 1;
    }
}

uint8_t DS18B20Sensor::readByte() {
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (readBit()) {
            byte |= (1 << i);
        }
    }
    return byte;
}

float DS18B20Sensor::readTemperatureC() {
    if (!reset()) {
        _deviceFound = false;
        // Disconnected indicator
        return -127.0f;
    }
    _deviceFound = true;

    // Start temperature conversion
    // Skip ROM command
    writeByte(0xCC);
    // Convert T command
    writeByte(0x44);

    // Wait for conversion (typ 750ms for 12-bit, or poll until line goes high)
    vTaskDelay(pdMS_TO_TICKS(750));

    if (!reset()) {
        _deviceFound = false;
        return -127.0f;
    }

    // Read Scratchpad
    // Skip ROM command
    writeByte(0xCC);
    // Read Scratchpad command
    writeByte(0xBE);

    uint8_t lsb = readByte();
    uint8_t msb = readByte();

    int16_t raw = (int16_t)((msb << 8) | lsb);
    return (float)raw * 0.0625f;
}
