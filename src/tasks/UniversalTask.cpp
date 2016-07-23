//
// Created by madrus on 17.07.16.
//

#include "utils.h"
#include "UniversalTask.h"
#include "JsonHelper.h"
#include "module_of_measurements.h"

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
        Module_of_measurements measure;
        _answer = new Poco::JSON::Object(true);
        _answer->set("name", getName());
        _answer->set("status", "ok");
        _answer->set("data", measure.GetNumChannels());
    } else if (_type == "disableMu") {
        common_utils::Gpio::set(115, false/*disable*/);
        _answer = new Poco::JSON::Object(true);
        _answer->set("name", getName());
        _answer->set("status", "ok");
    } else if (_type == "getTemp") {
        Module_of_measurements measure;
        unsigned int num_channel = std::stoi(_data);
        // заполняем темературу
        float temp = 0;
        int ret = measure.GetTemp(num_channel, temp);
        if (ret < 0) {
            throw Poco::Exception(Poco::format("Error get temp: %d", ret));
        }
        _answer = new Poco::JSON::Object(true);
        _answer->set("name", getName());
        _answer->set("status", "ok");
        _answer->set("data", std::to_string(temp));
    } else if (_type == "shutdown") {
        _answer = new Poco::JSON::Object(true);
        _answer->set("name", getName());
        _answer->set("status", "ok");
        Poco::Thread::sleep(100);
        int gpio_shutdown = 68;
        common_utils::Gpio::init(gpio_shutdown, true/*out - это хак!!! этот пин всегда работает на прием*/);
        common_utils::Gpio::set(gpio_shutdown, 1);
    }
    return true;
}
