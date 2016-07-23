//
// Created by madrus on 23.07.16.
//

#ifndef PSERVER_MEASURETASK_H
#define PSERVER_MEASURETASK_H

#include "AbstractTask.h"
#include "in5102_types.h"
#include "module_formation_signal.h"
#include "module_of_measurements.h"
#include <Poco/JSON/Object.h>

/**
 * реализует почастотное зондирование и поиск резонанса
 */
class MeasureTask : public AbstractTask {
public:
    MeasureTask(const Poco::JSON::Object::Ptr& config);

    virtual bool run() override;

private:
    // параметры датчика
    PropertySensor _propSensor;
    // параметры зондирования
    ParamProbing _paramProbing;
    // параметры зондирующего сигала
    ParamProbingSignal _paramSignal;
    // параметры захвата выходного сигнала
    PositionData _position;
    // тип сигнала
    std::string _typeSignal;
};

#endif //PSERVER_MEASURETASK_H
