//
// Created by madrus on 10.07.16.
//

#pragma once

#include "AbstractTask.h"
#include "in5102_types.h"
#include "module_formation_signal.h"
#include "module_of_measurements.h"
#include <Poco/JSON/Object.h>

/**
 * реализует почастотное зондирование и поиск резонанса
 */
class ScanResonanceTask : public AbstractTask {
public:
    ScanResonanceTask(const Poco::JSON::Object::Ptr& config);

    virtual bool run() override;

private:
    PropertySensor _propSensor;
    ParamProbing _paramProbing;
    PositionData _position;
    std::vector<ParamProbingSignal> _paramSignals;
};
