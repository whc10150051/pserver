//
// Created by madrus on 26.06.16.
//

#include <src/utils/JsonHelper.h>
#include "TaskFactory.h"
#include "FittingTask.h"

AbstractTask::Ptr TaskFactory::create(const Poco::JSON::Object::Ptr& config) {
    Poco::Logger& _logger(Poco::Logger::get("TaskFactory"));

    LOG_DEBUG("Create task begin");
    std::string taskStr = JsonHelper::getStringProperty(config, "task");
    LOG_DEBUG("Task is: %s", taskStr);
    if (taskStr == "fitting") {
        return std::make_shared<FittingTask>(config);
    } else if (taskStr == "measure") {
//        return std::make_shared<FittingTask>(config);
        return nullptr;
    }
    throw new Poco::Exception("TaskFactory::create");
}