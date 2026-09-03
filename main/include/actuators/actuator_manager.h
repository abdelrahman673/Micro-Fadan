#pragma once

#include "actuators/actuator_types.h"
#include "actuators/pump_controller.h"
#include "actuators/airflow_controller.h"
#include "sensors/sensor_types.h"

class ActuatorManager {
public:
    ActuatorManager();

    void begin();
    void update(const NormalizedSensorData& sensorData, uint32_t nowMs);

    void getStatus(ActuatorStatus& status, uint32_t nowMs) const;
    void printStatusReport(uint32_t nowMs) const;

    void manualPump(bool on, uint32_t nowMs);
    void manualAirflow(bool enable, bool air1, bool air2);

private:
    PumpController    _pump;
    AirflowController _airflow;
};

