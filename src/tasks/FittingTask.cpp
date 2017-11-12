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

std::string CURRENT_CONFIG = "";

FittingTask::FittingTask(const Poco::JSON::Object::Ptr& config) : AbstractTask(config, "FittingTask"), isAlreadyConfigure(true) {
    LOG_DEBUG("Begin FittingTask");
    std::string configStr = StringHelper::objectToString(config);
//    if (CURRENT_CONFIG != configStr) {
//        CURRENT_CONFIG = configStr;
        _propSensor.num_channel = JsonHelper::getIntProperty(config, "channel");
        LOG_DEBUG("channel: %d", (int) _propSensor.num_channel);
        _propSensor.num_relay = JsonHelper::getIntProperty(config, "relay");
        LOG_DEBUG("relay: %d", (int) _propSensor.num_relay);
        _propSensor.sens_is_hi = JsonHelper::getIntProperty(config, "sensishi");
        LOG_DEBUG("sensIsHi: %d", (int) _propSensor.sens_is_hi);
        _propSensor.volt_is_hi = JsonHelper::getIntProperty(config, "voltishi");
        LOG_DEBUG("voltIsHi: %d", (int) _propSensor.volt_is_hi);
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
        double tau = std::stof(tauStr);
        LOG_DEBUG("tau: %E", tau);
        std::string mfoStr = JsonHelper::getStringProperty(config, "mfo");
        double mfo = std::stof(mfoStr);
        LOG_DEBUG("mfo: %E", mfo);

        _paramSignal = ParamProbingSignal(tau, l, m, mfo);
//        isAlreadyConfigure = false;
//    }
    _position.n_start = JsonHelper::getIntProperty(config, "nstart");
    LOG_DEBUG("nStart: %d", (int) _position.n_start);
    _position.l_post = JsonHelper::getIntProperty(config, "lpost");
    LOG_DEBUG("lPost: %d", (int) _position.l_post);
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
    std::lock_guard<std::mutex> lock(fittingMutex);
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
    auto fitting = [&](DBL_VECTOR& data_out) {
        int ret;
        Module_of_measurements measure;
//        if (!isAlreadyConfigure) {
            LOG_DEBUG("SetupFitting");
            // уже не используем
            PAGC pagc;
            pagc.push_back(0x00);
            ret = measure.SetupGrinding(_paramSignal, _propSensor, pagc);
            if (ret < 0) {
                LOG_DEBUG("RunFitting ret = %d");
            }
//            Poco::Thread::sleep(500);
//        }
        LOG_DEBUG("RunFitting");
        ret = measure.RunGrinding(_propSensor.num_channel, _propSensor.num_relay, _paramProbing);
        if (ret < 0) {
            return ret;
        }
        LOG_DEBUG("GetMFEcho");
        DBL_VECTOR data_measure;
        ret = measure.GetMFEcho(_position, data_measure);
        if (ret < 0) {
            return ret;
        }
        LOG_DEBUG("smoof");
        data_out.clear();
        ret = smoof(data_measure, data_out, _sizeOut);
        return ret;
    };

    DBL_VECTOR dataOut;
    int numTry = 5;
    do {
        LOG_DEBUG("Num try fitting = %d", numTry);
        int ret = fitting(dataOut);
        if (ret < 0) {
            if (!--numTry) {
                throw Poco::Exception(Module_of_measurements::getMessageError(ret));
            } else {
                // можно чуть подождать перед следующим разом
                LOG_DEBUG("Wait 100 ms", numTry);
                Poco::Thread::sleep(100);
            }
        } else {
            numTry = 0;
        }
    } while (numTry);

    LOG_DEBUG("Create answer");
    _answer = new Poco::JSON::Object(true);
    _answer->set("name", getName());
    _answer->set("status", "ok");

    Poco::JSON::Array jsonArray;
    for (auto& item : dataOut) {
        jsonArray.add((int)item); // double тут не нужен
    }
    _answer->set("data", jsonArray);
#endif
    return true;
}
