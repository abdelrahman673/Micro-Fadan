#pragma once

#include "driver/gpio.h"

// Analog inputs use ADC1 (GPIO 32-39) so they remain functional when WiFi is active.

// Capacitive Moisture V1.2 (ADC1_CH6, Input-only)
#define PIN_SOIL_MOISTURE_ADC           GPIO_NUM_34

// Resistive Level (ADC1_CH7, Input-only)
#define PIN_WATER_LEVEL_ADC             GPIO_NUM_35

// Digital float switch (internal pull-up)
#define PIN_WATER_LEVEL_DIGITAL         GPIO_NUM_33

// DS18B20 1-Wire (requires 4.7k pull-up to 3.3V)
#define PIN_ONEWIRE_DS18B20             GPIO_NUM_4

// Shared I2C SDA (AHT21B & BH1750)
#define PIN_I2C_SDA                     GPIO_NUM_21

// Shared I2C SCL (AHT21B & BH1750)
#define PIN_I2C_SCL                     GPIO_NUM_22

#define I2C_ADDR_AHT21B                 0x38
#define I2C_ADDR_BH1750                 0x23

// Water pump relay/MOSFET
#define PIN_ACTUATOR_PUMP               GPIO_NUM_26

// Intake fan / louver
#define PIN_ACTUATOR_AIR_1              GPIO_NUM_27

// Exhaust fan
#define PIN_ACTUATOR_AIR_2              GPIO_NUM_14

#define PIN_STATUS_LED                  GPIO_NUM_2

// Relay polarity: true = active-LOW, false = active-HIGH
#define ACTUATOR_PUMP_ACTIVE_LOW        true
#define ACTUATOR_AIR_1_ACTIVE_LOW       true
#define ACTUATOR_AIR_2_ACTIVE_LOW       true
