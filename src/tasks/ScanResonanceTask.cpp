//
// Created by madrus on 10.07.16.
//

#include "ScanResonanceTask.h"


#include "src/utils/JsonHelper.h"
#include <Poco/Random.h>

ScanResonanceTask::ScanResonanceTask(const Poco::JSON::Object::Ptr& config) : AbstractTask(config, "FittingTask") {
    LOG_DEBUG("Begin FittingTask");


    _sensIsHi = JsonHelper::getIntProperty(config, "sensishi");
    LOG_DEBUG("sensIsHi: %d", (int) _sensIsHi);
    _voltIsHi = JsonHelper::getIntProperty(config, "voltishi");
    LOG_DEBUG("voltIsHi: %d", (int) _voltIsHi);
    _channel = JsonHelper::getIntProperty(config, "channel");
    LOG_DEBUG("channel: %d", (int) _channel);
    _relay = JsonHelper::getIntProperty(config, "relay");
    LOG_DEBUG("relay: %d", (int) _relay);
    _tau = JsonHelper::getIntProperty(config, "tau") * 10E-3;
    LOG_DEBUG("tau: %d", (int) _tau);
    _l = JsonHelper::getIntProperty(config, "l");
    LOG_DEBUG("l: %d", (int) _l);
    _m = JsonHelper::getIntProperty(config, "m");
    LOG_DEBUG("m: %d", (int) _m);
    _nStart = JsonHelper::getIntProperty(config, "nstart");
    LOG_DEBUG("nStart: %d", (int) _nStart);
    _lPost = JsonHelper::getIntProperty(config, "lpost");
    LOG_DEBUG("lPost: %d", (int) _lPost);
    _size = JsonHelper::getIntProperty(config, "size");
    LOG_DEBUG("size: %d", (int) _size);
}

bool ScanResonanceTask::run() {
// output
//    {
//        "name": "scan",
//        "size": 20,
//        "data": [29.9, 71.5, 106.4, 129.2, 144.0, 176.0, 135.6, 148.5, 16.4, 194.1, 95.6, 54.4]
//    }

    Poco::Random rnd;
    rnd.seed();
    std::vector<int> array;
    for (int i = 0; i < 250000; ++i) {
        array.push_back(rnd.next());
    }

    _answer = new Poco::JSON::Object(true);
    _answer->set("name", getName());
    _answer->set("status", "OK");
    _answer->set("size", array.size());

    Poco::JSON::Array jsonArray;
    for (auto& item : array) {
        jsonArray.add(item);
    }
    _answer->set("data", jsonArray);

    // эмуляция измерения, задержка
//    Poco::Thread::sleep(1000);

    return true;
}
