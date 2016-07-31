//
// Created by madrus on 10.07.16.
//

#include "ScanResonanceTask.h"
#include "Server.h"
#include "JsonHelper.h"
#include "utils.h"
#include "const.h"
#include <Poco/Random.h>
#include <string>

ScanResonanceTask::ScanResonanceTask(const Poco::JSON::Object::Ptr& config) : AbstractTask(config, "ScanResonanceTask") {
    LOG_DEBUG("Begin ScanResonanceTask");

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

    Poco::JSON::Array::Ptr pagc = JsonHelper::getArrayProperty(config, "pagc");
    size_t size = pagc->size();
    for (size_t idx = 0; idx < size; ++idx) {
        Poco::Dynamic::Var var = pagc->get(idx);
        _paramProbing.pagc.push_back(Poco::NumberParser::parseFloat(var.extract<std::string>()));
    }
    LOG_DEBUG("page size: %d", (int) _paramProbing.pagc.size());

    _position.n_start = JsonHelper::getIntProperty(config, "nstart");
    LOG_DEBUG("nStart: %d", (int) _position.n_start);
    _position.l_post = JsonHelper::getIntProperty(config, "lpost");
    LOG_DEBUG("lPost: %d", (int) _position.l_post);

    // принимаем многокомпонентный сигнал зондирования
    Poco::JSON::Array::Ptr signal = JsonHelper::getArrayProperty(config, "probingsignal");
    _scanSteps = signal->size();
    LOG_DEBUG("num steps: %d", _scanSteps);
    for (int idx = 0; idx < _scanSteps; ++idx) {
        Poco::Dynamic::Var var = signal->get(idx);
        Poco::JSON::Object::Ptr obj = var.extract<Poco::JSON::Object::Ptr>();
        int m = JsonHelper::getIntProperty(obj, "m");
        int l = JsonHelper::getIntProperty(obj, "l");
        double tau = Poco::NumberParser::parseFloat(JsonHelper::getStringProperty(obj, "tau"));  // секунды
        double mfo = Poco::NumberParser::parseFloat(JsonHelper::getStringProperty(obj, "mfo"));  // секунды
        _paramSignals.push_back(ParamProbingSignal(m, l, tau, mfo));
    }
}

bool ScanResonanceTask::run() {
// output
//    {
//        "name": "scan",
//        "status": "ok",
//        "imax":  9,
//        "maxafc": "194.1",
//        "afc": [29.9, 71.5, 106.4, 129.2, 144.0, 176.0, 135.6, 148.5, 16.4, 194.1, 95.6, 54.4]
//    }

    /*
     * почастотное зондирование
     */
    DBL_VECTOR vAfc;        // значение энергий почастотки
    double afc_fres = 1;    // afc на резонансной частоте
    int index_f_res = -1;   // Индекс резонансной частоты в массиве зондирования
    double x2_max = 0;      // максимальное значение квадрата эхосигнала на рез.частоте

    for (int step = 0; step < _scanSteps; ++step) {
        // суфикс
        LOG_DEBUG("number probing %d", step);
        Status::setStatus("scan " + std::to_string(step));
#if 1
        // настройка зондирования
        Module_of_measurements measure;
        // !!! пока зондируем однокомпонентным сигналом
        int ret = measure.Setup(_paramSignals[0], _propSensor);
        if (ret < 0) {
            throw Poco::Exception(Poco::format("Setup error: %d", ret));
        }

        // запуск измерения
        ret = measure.Run(_paramProbing);
        if (ret < 0) {
            throw Poco::Exception(Poco::format("Run error: %d", ret));
        }

        // Получение модуля СФ-эхосигнала (энергия)
        MODULE_MF_ECHO mod_data;
        ret = measure.GetModuleMFEcho(_position, mod_data);
        if (ret < 0) {
            throw Poco::Exception(Poco::format("GetSquareMFEcho error: %d", ret));
        }
#else
        Poco::Random rnd;
        rnd.seed();
        MODULE_MF_ECHO mod_data;
        int size = _position.l_post;
        for (int i = 0; i < size; ++i) {
            mod_data.push_back(rnd.nextDouble());
        }
        Poco::Thread::sleep(1000);
#endif

        double afc = 0;
//            double afc_accumulate = 0;
//            accumulate(sq_data.begin(), sq_data.end(), afc_accumulate);
        int size_mod_data = mod_data.size();
        for (int j = 0; j < size_mod_data; ++j) {
            afc += mod_data[j];
        }
        LOG_DEBUG("afc_%d = %f", step, afc);
        // добавляем энергию в массив
        vAfc.push_back(afc);
        // оценка afc
        if (afc > afc_fres) {
            afc_fres = afc;
            index_f_res = step;
            x2_max = *max_element(mod_data.begin(), mod_data.end());
        }
    }
    Status::clear();
    /*
     * Сканирование якобы прошло, считываем данные
     */
    _answer = new Poco::JSON::Object(true);
    _answer->set("name", getName());
    _answer->set("status", "ok");
    _answer->set("imax", index_f_res);
    _answer->set("maxafc", afc_fres);
    _answer->set("max2", x2_max);

    Poco::JSON::Array jsonArray;
    int size = vAfc.size();
    for (int i = 0; i < size; ++i) {
        jsonArray.add(vAfc[i] / afc_fres);
    }
    _answer->set("afc", jsonArray);
    return true;
}
