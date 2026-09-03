#include "actuators/airflow_controller.h"

AirflowController::AirflowController(gpio_num_t pinAir1, gpio_num_t pinAir2, bool activeLow1, bool activeLow2)
    : _pinAir1(pinAir1),
      _pinAir2(pinAir2),
      _activeLow1(activeLow1),
      _activeLow2(activeLow2),
      _air1Active(false),
      _air2Active(false),
      _currentReason(AirflowTriggerReason::NONE),
      _lastStateChangeMs(0),
      _manualOverride(false),
      _manualAir1(false),
      _manualAir2(false) {}

void AirflowController::begin() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << _pinAir1) | (1ULL << _pinAir2);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    applyOutputs(false, false);
}

void AirflowController::applyOutputs(bool air1, bool air2) {
    _air1Active = air1;
    _air2Active = air2;

    if (_activeLow1) {
        gpio_set_level(_pinAir1, air1 ? 0 : 1);
    } else {
        gpio_set_level(_pinAir1, air1 ? 1 : 0);
    }

    if (_activeLow2) {
        gpio_set_level(_pinAir2, air2 ? 0 : 1);
    } else {
        gpio_set_level(_pinAir2, air2 ? 1 : 0);
    }
}

void AirflowController::setManualOverride(bool enable, bool air1State, bool air2State) {
    _manualOverride = enable;
    _manualAir1 = air1State;
    _manualAir2 = air2State;

    if (enable) {
        _currentReason = AirflowTriggerReason::MANUAL_OVERRIDE;
        applyOutputs(air1State, air2State);
    }
}

void AirflowController::evaluate(const NormalizedSensorData& sensorData, uint32_t nowMs) {
    if (_manualOverride) return;

    if (nowMs - _lastStateChangeMs < AIRFLOW_MIN_CYCLE_TIME_MS && _lastStateChangeMs != 0) {
        // Prevent relay flapping
        return;
    }

    bool targetAir1 = false;
    bool targetAir2 = false;
    AirflowTriggerReason reason = AirflowTriggerReason::NONE;

    float temp = sensorData.ambientTemperatureC;
    float rh   = sensorData.ambientHumidityPercent;
    float vpd  = sensorData.vpdKpa;

    if (temp >= BASIL_GROWTH_TEMP_EXTREME_HIGH) {
        // High heat stress: dual fan exhaust
        targetAir1 = true;
        targetAir2 = true;
        reason = AirflowTriggerReason::HIGH_TEMPERATURE;
    } else if (rh >= BASIL_RH_CRITICAL_HIGH) {
        // High humidity (>85%): dual exhaust to prevent downy mildew
        targetAir1 = true;
        targetAir2 = true;
        reason = AirflowTriggerReason::HIGH_HUMIDITY;
    } else if (vpd < BASIL_VPD_CRITICAL_LOW) {
        // Low VPD (<0.65 kPa): stagnant humid air, activate intake/vent
        targetAir1 = true;
        targetAir2 = false;
        reason = AirflowTriggerReason::LOW_VPD;
    } else if (temp > (BASIL_GROWTH_TEMP_OPT_MAX - 1.0f)) {
        // Moderate temperature elevation: stage 1 cooling
        targetAir1 = true;
        targetAir2 = false;
        reason = AirflowTriggerReason::HIGH_TEMPERATURE;
    } else {
        // Hysteresis clearance check before shutting down
        if (_air1Active || _air2Active) {
            bool tempSafe = (temp <= (BASIL_GROWTH_TEMP_OPT_MAX - AIRFLOW_HYSTERESIS_TEMP_C));
            bool rhSafe   = (rh <= (BASIL_RH_CRITICAL_HIGH - AIRFLOW_HYSTERESIS_RH));
            bool vpdSafe  = (vpd >= BASIL_VPD_OPTIMAL_MIN);

            if (!tempSafe || !rhSafe || !vpdSafe) return;
        }
        targetAir1 = false;
        targetAir2 = false;
        reason = AirflowTriggerReason::NONE;
    }

    if (targetAir1 != _air1Active || targetAir2 != _air2Active) {
        applyOutputs(targetAir1, targetAir2);
        _currentReason = reason;
        _lastStateChangeMs = nowMs;
    }
}
