//
// Created by madrus on 10.07.16.
//

#include "ScanResonanceTask.h"
#include "src/utils/JsonHelper.h"
#include "utils.h"
#include "const.h"
#include <Poco/Random.h>

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
//    _paramProbing.pagc
    _position.n_start = JsonHelper::getIntProperty(config, "nstart");
    LOG_DEBUG("nStart: %d", (int) _position.n_start);
    _position.l_post = JsonHelper::getIntProperty(config, "lpost");
    LOG_DEBUG("lPost: %d", (int) _position.l_post);
    _scanSteps = JsonHelper::getIntProperty(config, "scansteps");
    LOG_DEBUG("scansteps: %d", (int) _scanSteps);

    // принимаем многокомпонентный сигнал зондирования
    Poco::JSON::Array::Ptr signal = JsonHelper::getArrayProperty(config, "probingsignal");
    size_t size = signal->size();
    for (size_t idx = 0; idx < size; ++idx) {
        Poco::Dynamic::Var var = signal->get(idx);
        Poco::JSON::Object::Ptr obj = var.extract<Poco::JSON::Object::Ptr>();
        double m = std::stod(JsonHelper::getStringProperty(obj, "m"));
        int l = std::stoi(JsonHelper::getStringProperty(obj, "l"));
        int tau = std::stoi(JsonHelper::getStringProperty(obj, "tau"));
        double mfo = std::stod(JsonHelper::getStringProperty(obj, "mfo"));
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

    // проверка на наличие спецфайлов
    if (common_utils::FileExists(common_const::FILENAME_INTERNAL_SYNCH.c_str())) {
        LOG_INFO("ACHTUNG!!! INTERNAL_SYNCH Enable");
    }
    if (common_utils::FileExists(common_const::FILENAME_CHECKMODE.c_str())) {
        LOG_INFO("ACHTUNG!!! CHECKMODE Enable");
    }

    try {
        /*
         * почастотное зондирование
         */
        DBL_VECTOR vAfc;       // значение энергий почастотки
        double afc_fres = 1;    // afc на резонансной частоте
        int index_f_res = -1;   // Индекс резонансной частоты в массиве зондирования
        double x2_max = 0;      // максимальное значение квадрата эхосигнала на рез.частоте

        for (int i = 0; i < _scanSteps; ++i) {
            // суфикс
            LOG_DEBUG("number probing %d", i);
#if 0
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
#endif

            double afc = 0;
//            double afc_accumulate = 0;
//            accumulate(sq_data.begin(), sq_data.end(), afc_accumulate);
            int size_mod_data = mod_data.size();
            for (int j = 0; j < size_mod_data; ++j) {
                afc += mod_data[j];
            }
            LOG_DEBUG("afc_%d = %f", i, afc);
            // добавляем энергию в массив
            vAfc.push_back(afc);
            // оценка afc
            if (afc > afc_fres) {
                afc_fres = afc;
                index_f_res = i;
                x2_max = *max_element(mod_data.begin(), mod_data.end());
            }
        }
        /*
         * Сканирование якобы прошло, считываем данные
         */
        _answer = new Poco::JSON::Object(true);
        _answer->set("name", getName());
        _answer->set("status", "ok");
        _answer->set("imax", index_f_res);
        _answer->set("maxafc", x2_max);

        Poco::JSON::Array jsonArray;
        int size = vAfc.size();
        for (int i = 0; i < size; ++i) {
            jsonArray.add(vAfc[i] / afc_fres);
        }
        _answer->set("afc", jsonArray);
    } catch (Poco::Exception& ex) {
        LOG_ERROR(ex.message());
        _answer = new Poco::JSON::Object(true);
        _answer->set("name", getName());
        _answer->set("status", "error");
        _answer->set("code", ex.displayText());
        return false;
    }
    return true;
}
