//
// Created by madrus on 26.06.16.
//

#include <src/utils/JsonHelper.h>
#include "TaskFactory.h"
#include "FittingTask.h"
#include "EchoTask.h"
#include "StatusTask.h"

AbstractTask::Ptr TaskFactory::create(const Poco::JSON::Object::Ptr& config) {
    Poco::Logger& _logger(Poco::Logger::get("TaskFactory"));

    LOG_DEBUG("Create task begin");
    std::string taskStr = JsonHelper::getStringProperty(config, "task");
    LOG_DEBUG("Task is: %s", taskStr);
    if (taskStr == "fitting") {
        return std::make_shared<FittingTask>(config);
    } else if (taskStr == "echo") {
        return std::make_shared<EchoTask>(config);
    } else if (taskStr == "status") {
        return std::make_shared<StatusTask>(config);
    } else if (taskStr == "measure") {
//        return std::make_shared<FittingTask>(config);
//        return nullptr;
    }
    LOG_DEBUG("Create exeption");
    throw Poco::Exception(Poco::format("TaskFactory: can not create a task name = %s", taskStr));
}