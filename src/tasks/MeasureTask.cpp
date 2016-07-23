//
// Created by madrus on 23.07.16.
//

#include "MeasureTask.h"

#include "ScanResonanceTask.h"
#include "JsonHelper.h"
#include "utils.h"
#include "const.h"
#include <Poco/Random.h>

MeasureTask::MeasureTask(const Poco::JSON::Object::Ptr& config) : AbstractTask(config, "MeasureTask") {
    LOG_DEBUG("Begin MeasureTask");
    _propSensor.num_channel = JsonHelper::getIntProperty(config, "channel");
    LOG_DEBUG("channel: %d", (int) _propSensor.num_channel);
    _propSensor.num_relay = JsonHelper::getIntProperty(config, "relay");
    LOG_DEBUG("relay: %d", (int) _propSensor.num_relay);
    _propSensor.sens_is_hi = JsonHelper::getIntProperty(config, "sensishi");
    LOG_DEBUG("sensIsHi: %d", (int) _propSensor.sens_is_hi);
    _propSensor.volt_is_hi = JsonHelper::getIntProperty(config, "voltishi");
    LOG_DEBUG("voltIsHi: %d", (int) _propSensor.volt_is_hi);
    _position.n_start = JsonHelper::getIntProperty(config, "nstart");
    LOG_DEBUG("nStart: %d", (int) _position.n_start);
    _position.l_post = JsonHelper::getIntProperty(config, "lpost");
    LOG_DEBUG("lPost: %d", (int) _position.l_post);
    _paramProbing.mode = JsonHelper::getIntProperty(config, "mode");
    LOG_DEBUG("mode: %d", (int) _paramProbing.mode);
    _paramProbing.n = JsonHelper::getIntProperty(config, "n");
    LOG_DEBUG("n: %d", (int) _paramProbing.n);
    _paramProbing.r = JsonHelper::getIntProperty(config, "r");
    LOG_DEBUG("r: %d", (int) _paramProbing.r);

    Poco::JSON::Array::Ptr pagc = JsonHelper::getArrayProperty(config, "pagc");
    size_t size = pagc->size();
    for (size_t idx = 0; idx < size; ++idx) {
        Poco::Dynamic::Var var = pagc->get(idx);
        _paramProbing.pagc.push_back(std::stod(var.extract<std::string>()));
    }
    LOG_DEBUG("page size: %d", (int) _paramProbing.pagc.size());

    int m = JsonHelper::getIntProperty(config, "m");
    LOG_DEBUG("m: %d", m);
    int l = JsonHelper::getIntProperty(config, "l");
    LOG_DEBUG("l: %d", l);
    double tau = std::stod(JsonHelper::getStringProperty(config, "tau")) * 10E-3; // миллисекунды
    LOG_DEBUG("tau: %f", tau);
    double mfo = std::stod(JsonHelper::getStringProperty(config, "mfo")) * 10E-3;
    LOG_DEBUG("mfo: %f", mfo);
    _paramSignal = ParamProbingSignal(m, l, tau, mfo);
    _typeSignal = JsonHelper::getStringProperty(config, "typeSignal");
}

bool MeasureTask::run() {
// output
//    {
//        "name": "measure",
//        "status": "ok",
//        "data": [29.9, 71.5, 106.4, 129.2, 144.0, 176.0, 135.6, 148.5, 16.4, 194.1, 95.6, 54.4]
//    }
//
#if 1
    // временная реализация без железа
    Poco::Random rnd;
    rnd.seed();
    std::vector<int> array;
    for (int i = 0; i < 250000; ++i) {
        array.push_back(rnd.next());
    }

    _answer = new Poco::JSON::Object(true);
    _answer->set("name", getName());
    _answer->set("status", "ok");

    Poco::JSON::Array jsonArray;
    for (auto& item : array) {
        jsonArray.add(item);
    }
    _answer->set("data", jsonArray);

    // эмуляция измерения, задержка
//    Poco::Thread::sleep(1000);
#else
    // настройка зондирования
    LOG_DEBUG("Setup");
    Module_of_measurements measure;
    int ret = measure.Setup(_paramSignal, _propSensor);
    if (ret < 0) {
        throw Poco::Exception(Poco::format("Error setup measure: %d", ret));
    }
    LOG_DEBUG("Run");
    ret = measure.Run(_paramProbing);
    if (ret < 0) {
        throw Poco::Exception(Poco::format("Error start measure: %d", ret));
    }
    LOG_DEBUG("Get signal");
    DBL_VECTOR data_measure;
    if (_typeSignal == "echo") {
        ret = measure.GetMFEcho(_position, data_measure);
    } else if (_typeSignal == "env") {
        ret = measure.GetEnvMFEcho(_position, data_measure);
    } else if (_typeSignal == "square") {
        ret = measure.GetSquareMFEcho(_position, data_measure);
    } else if (_typeSignal == "module") {
        ret = measure.GetModuleMFEcho(_position, data_measure);
    } else if (_typeSignal == "att") {
        ret = measure.GetAttMFEcho(_position, data_measure);
    }
    if (ret < 0) {
        throw Poco::Exception(Poco::format("Error get signal (%s) measure: %d", _typeSignal, ret));
    }

    LOG_DEBUG("Create answer");
    _answer = new Poco::JSON::Object(true);
    _answer->set("name", getName());
    _answer->set("status", "ok");

    Poco::JSON::Array jsonArray;
    for (auto& item : data_measure) {
        jsonArray.add(item);
    }
    _answer->set("data", jsonArray);
#endif
    return true;
}