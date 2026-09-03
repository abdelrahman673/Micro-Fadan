#include "actuators/actuator_manager.h"
#include "esp_log.h"

static const char *TAG = "ACTUATOR";

ActuatorManager::ActuatorManager() {}

void ActuatorManager::begin() {
    ESP_LOGI(TAG, "Initializing pump and airflow outputs...");
    _pump.begin();
    _airflow.begin();
}

void ActuatorManager::update(const NormalizedSensorData& sensorData, uint32_t nowMs) {
    _pump.evaluate(sensorData, nowMs);
    _airflow.evaluate(sensorData, nowMs);
}

void ActuatorManager::getStatus(ActuatorStatus& status, uint32_t nowMs) const {
    status.pumpActive = _pump.isRunning();
    status.pumpState  = _pump.getState();
    status.currentPumpRunDurationMs = _pump.getCurrentRunDurationMs(nowMs);
    status.totalWateringSessions    = _pump.getTotalSessions();
    status.estimatedWaterDispensedMl = _pump.getEstimatedDispensedMl();
    status.pumpLockoutActive        = (_pump.getState() == ActuatorState::STATE_SAFETY_LOCKOUT);
    status.pumpStatusMessage        = _pump.getStatusMessage();

    status.air1Active = _airflow.isAir1Active();
    status.air2Active = _airflow.isAir2Active();
    status.air1State  = status.air1Active ? ActuatorState::STATE_ON : ActuatorState::STATE_OFF;
    status.air2State  = status.air2Active ? ActuatorState::STATE_ON : ActuatorState::STATE_OFF;
    status.airReason  = _airflow.getReason();
}

void ActuatorManager::printStatusReport(uint32_t nowMs) const {
    ActuatorStatus st;
    getStatus(st, nowMs);

    const char* reasonStr = "IDLE";
    switch (st.airReason) {
        case AirflowTriggerReason::HIGH_TEMPERATURE: reasonStr = "HIGH TEMPERATURE"; break;
        case AirflowTriggerReason::HIGH_HUMIDITY:    reasonStr = "HIGH HUMIDITY (Anti-Mold)"; break;
        case AirflowTriggerReason::LOW_VPD:          reasonStr = "LOW VPD (Stagnant Air)"; break;
        case AirflowTriggerReason::MANUAL_OVERRIDE:  reasonStr = "MANUAL OVERRIDE"; break;
        default: reasonStr = "IDLE (Optimal Climate)"; break;
    }

    ESP_LOGI(TAG, "Pump: %s (%s) | Fan1: %s | Fan2: %s | Airflow Reason: %s",
             st.pumpActive ? "ON" : "OFF", st.pumpStatusMessage,
             st.air1Active ? "ON" : "OFF",
             st.air2Active ? "ON" : "OFF",
             reasonStr);
}

void ActuatorManager::manualPump(bool on, uint32_t nowMs) {
    if (on) _pump.manualTurnOn(nowMs);
    else    _pump.manualTurnOff(nowMs);
}

void ActuatorManager::manualAirflow(bool enable, bool air1, bool air2) {
    _airflow.setManualOverride(enable, air1, air2);
}

