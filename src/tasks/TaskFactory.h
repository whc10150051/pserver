//
// Created by madrus on 26.06.16.
//

#ifndef PSERVER_TASKFACTORY_H
#define PSERVER_TASKFACTORY_H


#include <Poco/JSON/Object.h>
#include "AbstractTask.h"

namespace TaskFactory {
    AbstractTask::Ptr create(const Poco::JSON::Object::Ptr& config);
};


#endif //PSERVER_TASKFACTORY_H
