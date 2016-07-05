//
// Created by madrus on 26.06.16.
//

#include "AbstractTask.h"

AbstractTask::AbstractTask(const Poco::JSON::Object::Ptr& config, const std::string& name) :
        _logger(Poco::Logger::get(name)), _config(config), _name(name) {
}

const Poco::JSON::Object::Ptr &AbstractTask::getAnswer() const {
    return _answer;
}

const std::string &AbstractTask::getName() const {
    return _name;
}
