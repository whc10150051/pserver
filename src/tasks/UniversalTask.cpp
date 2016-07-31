//
// Created by madrus on 17.07.16.
//

#include "Const.h"
#include "utils.h"
#include "UniversalTask.h"
#include "JsonHelper.h"
#include "module_of_measurements.h"
#include <fstream>
#include <string>

UniversalTask::UniversalTask(const Poco::JSON::Object::Ptr& config) : AbstractTask(config, "UniversalTask") {
    LOG_DEBUG("Begin UniversalTask");
    _type = JsonHelper::getStringProperty(config, "type");
    if (JsonHelper::hasProperty(config, "data")) {
        _data = JsonHelper::getStringProperty(config, "data");
    }
}

bool UniversalTask::run() {
// output
//    {
//        "name": "get",
//        "data": "value"
//    }
    if (_type == "getNumСhannels") {

        LOG_DEBUG("Begin subtask %s", _type);
        _answer = new Poco::JSON::Object(true);
        _answer->set("name", getName());
        _answer->set("type", _type);
        _answer->set("status", "ok");
#if 0
        _answer->set("data", 8);
#else
        Module_of_measurements measure;
        _answer->set("data", measure.GetNumChannels());
#endif
        LOG_DEBUG("End subtask %s", _type);
    } else


    if (_type == "disableMu") {

        LOG_DEBUG("Begin subtask %s", _type);
#if 1
        common_utils::Gpio::set(115, false/*disable*/);
#endif
        _answer = new Poco::JSON::Object(true);
        _answer->set("name", getName());
        _answer->set("type", _type);
        _answer->set("status", "ok");
        LOG_DEBUG("End subtask %s", _type);
    } else


    if (_type == "getTemp") {
        LOG_DEBUG("Begin subtask %s", _type);
        float temp = 0;
        unsigned int num_channel = std::stoi(_data);
        // заполняем темературу
#if 1
        Module_of_measurements measure;
        int ret = measure.GetTemp(num_channel, temp);
        if (ret < 0) {
            throw Poco::Exception(Poco::format("Error get temp: %d", ret));
        }
#endif
        _answer = new Poco::JSON::Object(true);
        _answer->set("name", getName());
        _answer->set("type", _type);
        _answer->set("status", "ok");
        _answer->set("data", std::to_string(temp));
        LOG_DEBUG("End subtask %s", _type);
    } else


    if (_type == "shutdown") {

        LOG_DEBUG("Begin subtask %s", _type);
        _answer = new Poco::JSON::Object(true);
        _answer->set("name", getName());
        _answer->set("type", _type);
        _answer->set("status", "ok");
#if 1
        int gpio_shutdown = 68;
        common_utils::Gpio::init(gpio_shutdown, true/*out - это хак!!! этот пин всегда работает на прием*/);
        common_utils::Gpio::set(gpio_shutdown, 1);
#else
        Poco::Thread::sleep(100);
#endif
        LOG_DEBUG("End subtask %s", _type);
    }
    return true;
}
