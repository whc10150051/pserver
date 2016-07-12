//
// Created by madrus on 10.07.16.
//

#pragma once

#include "AbstractTask.h"
#include <Poco/JSON/Object.h>

class StatusTask : public AbstractTask {
public:
    StatusTask(const Poco::JSON::Object::Ptr& config);

    virtual bool run() override;
};
