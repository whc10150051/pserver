//
// Created by madrus on 17.07.16.
//

#include "GetTask.h"
#include "JsonHelper.h"

GetTask::GetTask(const Poco::JSON::Object::Ptr& config) : AbstractTask(config, "GetTask") {
    LOG_DEBUG("Begin GetTask");
    _data = JsonHelper::getStringProperty(config, "data");
}

bool GetTask::run() {
// output
//    {
//        "name": "get",
//        "data": "text echo"
//    }
//    DT_NUM_CHANNELS
//            DT_SQUARE_MF_ECHO
//    DT_MF_ECHO
//            DT_ATT_MF_ECHO
//    DT_ENV_MF_ECHO
//            DT_AFC
//    _answer = new Poco::JSON::Object(true);
//    _answer->set("name", getName());
//    _answer->set("status", "ok");
//    _answer->set("data", _data);

    return true;
}
