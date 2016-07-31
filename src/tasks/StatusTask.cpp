//
// Created by madrus on 10.07.16.
//

#include "Server.h"
#include "StatusTask.h"
#include <Poco/Random.h>
#include <src/Voltage.h>

StatusTask::StatusTask(const Poco::JSON::Object::Ptr& config) : AbstractTask(config, "StatusTask") {
    LOG_DEBUG("Begin EchoTask");
}

bool StatusTask::run() {
// output
//    {
//        "name": "status",
//        "data": "text echo"
//    }

    _answer = new Poco::JSON::Object(true);
    _answer->set("name", getName());
    _answer->set("status", "ok");
#if 1
    Poco::Random rnd;
    rnd.seed();
    _answer->set("voltage", rnd.next(20));
#else

    _answer->set("voltage", Voltage::getVoltage());
#endif
    _answer->set("DT_CHARGING", 1);
    _answer->set("DT_RECOVERY", 1);
    _answer->set("state", Status::getStatus()); // строка может передавать состояние какого либо процесса, например номер зондируемой компоненты в почастотке
    return true;
}
