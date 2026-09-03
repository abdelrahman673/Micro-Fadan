#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "actuators/actuator_types.h"
#include "sensors/sensor_types.h"
#include "driver/gpio.h"
#include "config.h"

class AirflowController {
public:
    AirflowController(gpio_num_t pinAir1 = PIN_ACTUATOR_AIR_1,
                      gpio_num_t pinAir2 = PIN_ACTUATOR_AIR_2,
                      bool activeLow1 = ACTUATOR_AIR_1_ACTIVE_LOW,
                      bool activeLow2 = ACTUATOR_AIR_2_ACTIVE_LOW);

    void begin();
    void evaluate(const NormalizedSensorData& sensorData, uint32_t nowMs);

    bool isAir1Active() const { return _air1Active; }
    bool isAir2Active() const { return _air2Active; }
    AirflowTriggerReason getReason() const { return _currentReason; }

    void setManualOverride(bool enable, bool air1State = false, bool air2State = false);

private:
    void applyOutputs(bool air1, bool air2);

    gpio_num_t _pinAir1;
    gpio_num_t _pinAir2;
    bool       _activeLow1;
    bool       _activeLow2;

    bool       _air1Active;
    bool       _air2Active;
    AirflowTriggerReason _currentReason;

    uint32_t _lastStateChangeMs;
    bool     _manualOverride;
    bool     _manualAir1;
    bool     _manualAir2;
};

