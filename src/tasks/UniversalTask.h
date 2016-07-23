//
// Created by madrus on 17.07.16.
//

#pragma once

#include "AbstractTask.h"
#include <Poco/JSON/Object.h>

class UniversalTask : public AbstractTask {
public:
    UniversalTask(const Poco::JSON::Object::Ptr& config);

    virtual bool run() override;

private:
    std::string _type;
    std::string _data;
};
