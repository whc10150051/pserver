//
// Created by madrus on 23.07.16.
//

#include "MeasureTask.h"

#include "ScanResonanceTask.h"
#include "JsonHelper.h"
#include "utils.h"
#include "const.h"
#include <Poco/Random.h>
#include <string>
#include <numeric>

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
        _paramProbing.pagc.push_back(Poco::NumberParser::parseFloat(var.extract<std::string>()));
    }
    LOG_DEBUG("page size: %d", (int) _paramProbing.pagc.size());

// принимаем многокомпонентный сигнал зондирования
    Poco::JSON::Array::Ptr signal = JsonHelper::getArrayProperty(config, "probingsignal");
    size = signal->size();
    LOG_DEBUG("num steps: %d", (int) size);
    std::vector<ParamProbingSignal> _paramSignals;
    for (size_t idx = 0; idx < size; ++idx) {
        Poco::Dynamic::Var var = signal->get(idx);
        Poco::JSON::Object::Ptr obj = var.extract<Poco::JSON::Object::Ptr>();
        int m = JsonHelper::getIntProperty(obj, "m");
        int l = JsonHelper::getIntProperty(obj, "l");
        std::string tauStr = JsonHelper::getStringProperty(obj, "tau");  // секунды
        double tau = std::stof(tauStr);
        std::string mfoStr = JsonHelper::getStringProperty(obj, "mfo");  // секунды
        double mfo = std::stof(mfoStr);
        LOG_DEBUG("m: %d l: %d tau: %E mfo: %E", m, l, tau, mfo);
        _paramSignals.push_back(ParamProbingSignal(tau, l, m, mfo));
    }
    _paramSignal = _paramSignals[0];
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
#if 0
    // временная реализация без железа
    Poco::Random rnd;
    double acc = 1000;
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
    auto measuring = [&](DBL_VECTOR& data_measure, double& acc) {
        int ret;
        LOG_DEBUG("Setup measure");
        Module_of_measurements measure;
        ret = measure.Setup(_paramSignal, _propSensor);
        if (ret < 0) {
            return ret;
        }
        LOG_DEBUG("Run");
        ret = measure.Run(_paramProbing);
        if (ret < 0) {
            return ret;
        }
        LOG_DEBUG("Get signal");
        if (_typeSignal == "echo") {
            ret = measure.GetMFEcho(_position, data_measure);
        } else if (_typeSignal == "env") {
            ret = measure.GetEnvMFEcho(_position, data_measure);
        } else if (_typeSignal == "square") {
            ret = measure.GetSquareMFEcho(_position, data_measure);
        } else if (_typeSignal == "env_afc") {
            ret = measure.GetEnvMFEcho(_position, data_measure);
            if (ret >= 0) {
                DBL_VECTOR dataAfc;
                ret = measure.GetModuleMFEcho(_position, dataAfc);
                std::accumulate(dataAfc.begin(), dataAfc.end(), acc);
            }
        } else if (_typeSignal == "module") {
            ret = measure.GetModuleMFEcho(_position, data_measure);
        } else if (_typeSignal == "att") {
            ret = measure.GetAttMFEcho(_position, data_measure);
        }
        return ret;
    };

    double acc = 0.;
    DBL_VECTOR dataMeasure;
    int numTry = 5;
    do {
        LOG_DEBUG("Num try measuring = %d", numTry);
        int ret = measuring(dataMeasure, acc);
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
    for (auto& item : dataMeasure) {
        jsonArray.add(item);
    }
    _answer->set("data", jsonArray);
#endif
    if (_typeSignal == "env_afc") {
        _answer->set("afc", std::to_string(acc));
    }
    return true;
}
