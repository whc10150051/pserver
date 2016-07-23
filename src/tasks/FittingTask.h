//
// Created by madrus on 26.06.16.
//

#ifndef PSERVER_FITTINGTASK_H
#define PSERVER_FITTINGTASK_H


#include "AbstractTask.h"
#include "module_formation_signal.h"
#include "module_of_measurements.h"

class FittingTask : public AbstractTask {
public:
    FittingTask(const Poco::JSON::Object::Ptr& config);

    virtual bool run() override;

private:
    PositionData _position;
    ParamProbing _paramProbing;
    PropertySensor _propSensor;
    ParamProbingSignal _paramSignal;
    int _sizeOut;
};

#endif //PSERVER_FITTINGTASK_H
