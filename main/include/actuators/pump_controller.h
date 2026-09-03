#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "actuators/actuator_types.h"
#include "sensors/sensor_types.h"
#include "driver/gpio.h"
#include "config.h"

class PumpController {
public:
    PumpController(gpio_num_t pin = PIN_ACTUATOR_PUMP, bool activeLow = ACTUATOR_PUMP_ACTIVE_LOW);

    void begin();
    void evaluate(const NormalizedSensorData& sensorData, uint32_t nowMs);
    void manualTurnOn(uint32_t nowMs);
    void manualTurnOff(uint32_t nowMs);

    bool isRunning() const { return _isRunning; }
    ActuatorState getState() const { return _state; }
    uint32_t getCurrentRunDurationMs(uint32_t nowMs) const;
    uint32_t getTotalSessions() const { return _totalSessions; }
    float getEstimatedDispensedMl() const { return _totalWaterDispensedMl; }
    const char* getStatusMessage() const { return _statusMessage; }

private:
    void activateHardware(bool on);

    gpio_num_t    _pin;
    bool          _activeLow;
    bool          _isRunning;
    ActuatorState _state;

    uint32_t _sessionStartTimeMs;
    uint32_t _lastStopTimeMs;
    uint32_t _totalSessions;
    float    _totalWaterDispensedMl;

    const char* _statusMessage;
};

