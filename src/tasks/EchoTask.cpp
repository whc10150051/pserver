//
// Created by madrus on 09.07.16.
//

#include "EchoTask.h"
#include "JsonHelper.h"

EchoTask::EchoTask(const Poco::JSON::Object::Ptr& config) : AbstractTask(config, "EchoTask") {
    LOG_DEBUG("Begin EchoTask");
    _data = JsonHelper::getStringProperty(config, "data");
}

bool EchoTask::run() {
// output
//    {
//        "name": "echo",
//        "data": "text echo"
//    }

    _answer = new Poco::JSON::Object(true);
    _answer->set("name", getName());
    _answer->set("status", "ok");
    _answer->set("data", _data);

    return true;
}
