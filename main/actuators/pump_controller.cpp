#include "actuators/pump_controller.h"

PumpController::PumpController(gpio_num_t pin, bool activeLow)
    : _pin(pin),
      _activeLow(activeLow),
      _isRunning(false),
      _state(ActuatorState::STATE_OFF),
      _sessionStartTimeMs(0),
      _lastStopTimeMs(0),
      _totalSessions(0),
      _totalWaterDispensedMl(0.0f),
      _statusMessage("IDLE") {}

void PumpController::begin() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << _pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    activateHardware(false);
}

void PumpController::activateHardware(bool on) {
    if (_activeLow) {
        gpio_set_level(_pin, on ? 0 : 1);
    } else {
        gpio_set_level(_pin, on ? 1 : 0);
    }
}

uint32_t PumpController::getCurrentRunDurationMs(uint32_t nowMs) const {
    if (_isRunning) {
        return nowMs - _sessionStartTimeMs;
    }
    return 0;
}

void PumpController::evaluate(const NormalizedSensorData& sensorData, uint32_t nowMs) {
    // Reservoir empty lockout
    if (!sensorData.isWaterReservoirSafe) {
        if (_isRunning) {
            activateHardware(false);
            _isRunning = false;
            _lastStopTimeMs = nowMs;
        }
        _state = ActuatorState::STATE_SAFETY_LOCKOUT;
        _statusMessage = "LOCKOUT: Water reservoir empty! Refill required.";
        return;
    }

    // Max runtime cutoff
    if (_isRunning) {
        uint32_t currentRunTime = nowMs - _sessionStartTimeMs;
        if (currentRunTime >= PUMP_MAX_RUN_TIME_MS) {
            activateHardware(false);
            _isRunning = false;
            _lastStopTimeMs = nowMs;
            _state = ActuatorState::STATE_SAFETY_LOCKOUT;
            _statusMessage = "LOCKOUT: Max runtime reached! Cooldown active.";

            float dispensed = (currentRunTime / 1000.0f) * PUMP_FLOW_RATE_ML_PER_SEC;
            _totalWaterDispensedMl += dispensed;
            return;
        }

        if (sensorData.soilMoisturePercent >= BASIL_SOIL_MOISTURE_IRR_STOP) {
            activateHardware(false);
            _isRunning = false;
            _lastStopTimeMs = nowMs;
            _state = ActuatorState::STATE_OFF;
            _statusMessage = "Target moisture reached. Pump turned off.";

            float dispensed = (currentRunTime / 1000.0f) * PUMP_FLOW_RATE_ML_PER_SEC;
            _totalWaterDispensedMl += dispensed;
            return;
        }

        _statusMessage = "IRRIGATING...";
        return;
    }

    // Mandatory absorption cooldown
    if (nowMs - _lastStopTimeMs < PUMP_MIN_COOLDOWN_MS && _lastStopTimeMs != 0) {
        _state = ActuatorState::STATE_OFF;
        _statusMessage = "Resting: Soil moisture absorption cooldown.";
        return;
    }

    if (sensorData.soilMoisturePercent < BASIL_SOIL_MOISTURE_IRR_START) {
        _isRunning = true;
        _sessionStartTimeMs = nowMs;
        _totalSessions++;
        _state = ActuatorState::STATE_ON;
        _statusMessage = "Irrigation triggered: Soil dry.";
        activateHardware(true);
    } else {
        _state = ActuatorState::STATE_OFF;
        _statusMessage = "IDLE: Soil moisture optimal.";
    }
}

void PumpController::manualTurnOn(uint32_t nowMs) {
    _isRunning = true;
    _sessionStartTimeMs = nowMs;
    _state = ActuatorState::STATE_ON;
    _statusMessage = "MANUAL OVERRIDE: ON";
    activateHardware(true);
}

void PumpController::manualTurnOff(uint32_t nowMs) {
    _isRunning = false;
    _lastStopTimeMs = nowMs;
    _state = ActuatorState::STATE_OFF;
    _statusMessage = "MANUAL OVERRIDE: OFF";
    activateHardware(false);
}

