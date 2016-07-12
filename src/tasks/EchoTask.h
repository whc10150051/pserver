//
// Created by madrus on 09.07.16.
//

#pragma once

#include "AbstractTask.h"
#include <Poco/JSON/Object.h>

class EchoTask : public AbstractTask {
public:
    EchoTask(const Poco::JSON::Object::Ptr& config);

    virtual bool run() override;

private:
    std::string _data;
};
