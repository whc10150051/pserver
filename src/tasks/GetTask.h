//
// Created by madrus on 17.07.16.
//

#pragma once

#include "AbstractTask.h"
#include <Poco/JSON/Object.h>

class GetTask : public AbstractTask {
public:
    GetTask(const Poco::JSON::Object::Ptr& config);

    virtual bool run() override;

private:
    std::string _data;
};
