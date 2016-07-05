//
// Created by madrus on 26.06.16.
//

#ifndef PSERVER_TASK_H
#define PSERVER_TASK_H

#include "src/utils/Logger.h"
#include <Poco/Util/JSONConfiguration.h>
#include <Poco/JSON/Object.h>
#include <memory>

class AbstractTask {
public:
    AbstractTask(const Poco::JSON::Object::Ptr& config, const std::string& name);
    virtual ~AbstractTask() {}

    virtual bool run() = 0;

    const Poco::JSON::Object::Ptr& getAnswer() const;

    const std::string& getName() const;

    using Ptr = std::shared_ptr<AbstractTask>;

protected:
    Poco::Logger& _logger;
    Poco::JSON::Object::Ptr _answer = nullptr;
    Poco::Util::JSONConfiguration _config;

private:
    std::string _name;
};

#endif //PSERVER_FITTING_H
