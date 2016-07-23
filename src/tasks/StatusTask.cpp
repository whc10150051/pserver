//
// Created by madrus on 10.07.16.
//

#include <Poco/Random.h>
#include "StatusTask.h"

StatusTask::StatusTask(const Poco::JSON::Object::Ptr& config) : AbstractTask(config, "StatusTask") {
    LOG_DEBUG("Begin EchoTask");
}

bool StatusTask::run() {
// output
//    {
//        "name": "status",
//        "data": "text echo"
//    }

    Poco::Random rnd;
    rnd.seed();

    _answer = new Poco::JSON::Object(true);
    _answer->set("name", getName());
    _answer->set("status", "ok");
    _answer->set("voltage", rnd.next(20));
    _answer->set("temp", rnd.next(40));
    _answer->set("DT_CHARGING", 1);
    _answer->set("DT_RECOVERY", 1);
    _answer->set("state", ""); // строка может передавать состояние какого либо процесса, например номер зондируемой компоненты в почастотке

    return true;
}
