#pragma once

#include <stdint.h>
#include <stdbool.h>

enum class ActuatorState {
    STATE_OFF,
    STATE_ON,
    STATE_FAULT,
    STATE_SAFETY_LOCKOUT
};

enum class AirflowTriggerReason {
    NONE,
    HIGH_TEMPERATURE,
    HIGH_HUMIDITY,
    LOW_VPD,
    MANUAL_OVERRIDE
};

struct ActuatorStatus {
    bool pumpActive;
    bool air1Active;
    bool air2Active;

    ActuatorState pumpState;
    ActuatorState air1State;
    ActuatorState air2State;

    AirflowTriggerReason airReason;

    uint32_t currentPumpRunDurationMs;
    uint32_t totalWateringSessions;
    float    estimatedWaterDispensedMl;

    // True if safety timeout or reservoir empty
    bool pumpLockoutActive;
    const char* pumpStatusMessage;
};
