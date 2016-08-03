//
// Created by madrus on 26.06.16.
//

#include "FittingTask.h"
#include "JsonHelper.h"
#include "in5102_types.h"
#include "module_of_measurements.h"
#include <Poco/Random.h>
#include <Poco/Exception.h>
#include <string>
#include <cstdio>
#include <src/utils/StringHelper.h>

FittingTask::FittingTask(const Poco::JSON::Object::Ptr& config) : AbstractTask(config, "FittingTask") {
    LOG_DEBUG("Begin FittingTask");
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
    // по сути глушим, для притирки этого не нужно
    _paramProbing.pagc = {0};

    int m = JsonHelper::getIntProperty(config, "m");
    LOG_DEBUG("m: %d", m);
    int l = JsonHelper::getIntProperty(config, "l");
    LOG_DEBUG("l: %d", l);
    std::string tauStr = JsonHelper::getStringProperty(config, "tau");
    LOG_DEBUG("tauStr: %s", tauStr);
    double tau = std::stof(tauStr);
    LOG_DEBUG("tau: %E", tau);
    std::string mfoStr = JsonHelper::getStringProperty(config, "mfo");
    LOG_DEBUG("mfoStr: %s", mfoStr);
    double mfo = std::stof(mfoStr);
    LOG_DEBUG("mfo: %E", mfo);

    _paramSignal = ParamProbingSignal(tau, l, m, mfo);

    _sizeOut = JsonHelper::getIntProperty(config, "size");
    LOG_DEBUG("size: %d", _sizeOut);
}


// Усреднение
int smoof(const DBL_VECTOR& data_in, DBL_VECTOR& data_out, int size) {

    int size_data_measure = data_in.size();
    if (!size || !size_data_measure || size > size_data_measure) {
        return E_MSERVER_SMOOF;
    }
    // выхлоп в файл (а так же усреднение)
    int window_size = 0; // размер окна
    if (size != 0) {
        window_size = size_data_measure / size;
    }
    for (int i = 0; i < size; ++i) {
        double value = 0;
        int begin_window = floor(i * window_size);
        for (int j = 0; j < window_size; ++j) {
            value += data_in[begin_window + j];
        }

        data_out.push_back(value / window_size);
    }
    return E_OK;
}

bool FittingTask::run() {
// output
//    {
//        "name": "fitting",
//        "data": [29.9, 71.5, 106.4, 129.2, 144.0, 176.0, 135.6, 148.5, 16.4, 194.1, 95.6, 54.4]
//    }

#if 0
    // временная реализация без железа
    Poco::Random rnd;
    rnd.seed();
    std::vector<double> array;
    for (int i = 0; i < (int)_position.l_post; ++i) {
        array.push_back(rnd.next());
    }

    DBL_VECTOR data_out;
    smoof(array, data_out, _sizeOut);

    _answer = new Poco::JSON::Object(true);
    _answer->set("name", getName());
    _answer->set("status", "ok");

    Poco::JSON::Array jsonArray;
    for (auto& item : data_out) {
        jsonArray.add(item);
    }
    _answer->set("data", jsonArray);

    // эмуляция измерения, задержка
//    Poco::Thread::sleep(1000);
#else
    // настройка зондирования
    LOG_DEBUG("SetupGrinding");
    Module_of_measurements measure;
    // уже не используем
    PAGC pagc;
    pagc.push_back(0x00);
    int ret = measure.SetupGrinding(_paramSignal, _propSensor, pagc);
    if (ret < 0) {
        throw Poco::Exception(Poco::format("Error setup fitting: %d", ret));
    }
    LOG_DEBUG("RunGrinding");
    ret = measure.RunGrinding(_propSensor.num_channel, _propSensor.num_relay, _paramProbing);
    if (ret < 0) {
        throw Poco::Exception(Poco::format("Error start fitting: %d", ret));
    }
    LOG_DEBUG("GetMFEcho");
    DBL_VECTOR data_measure;
    ret = measure.GetMFEcho(_position, data_measure);
    if (ret < 0) {
        throw Poco::Exception(Poco::format("Error get signal fitting: %d", ret));
    }
    LOG_DEBUG("smoof");
    DBL_VECTOR data_out;
    ret = smoof(data_measure, data_out, _sizeOut);
    if (ret < 0) {
        throw Poco::Exception(Poco::format("Error smoof signal fitting: %d", ret));
    }

    LOG_DEBUG("Create answer");
    _answer = new Poco::JSON::Object(true);
    _answer->set("name", getName());
    _answer->set("status", "ok");

    Poco::JSON::Array jsonArray;
    for (auto& item : data_out) {
        jsonArray.add(item);
    }
    _answer->set("data", jsonArray);
#endif
    return true;
}

