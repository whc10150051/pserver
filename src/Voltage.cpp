//
// Created by madrus on 24.07.16.
//

#include "Voltage.h"
#include "Const.h"
#include <fstream>

void Voltage::run() {
    LOG_DEBUG("Begin loop measuring voltage");
    while (1) {
        try {
            // Выдает напряжение батареи
            /*
             * читаем файл с калибровочным коэфициентом и со значением АЦП
             * перемножаем значения и выдаем значение
             */
            unsigned int adc = 0;   // отсчеты АЦП
            double adc_config = 1;  // коэфф. пересчета
#if 1
            // читаем калибровочный коэффициент
            std::ifstream in_adc_conf(ADC_CONF_STR.c_str());
            if (in_adc_conf.is_open()) {
                in_adc_conf >> adc_config;
                in_adc_conf.close();
            } else {
                /*
                 * файла нет и возвращает единичный коэффициент
                 */
                LOG_WARNING("File %s not found. Calibration factor set by default", ADC_CONF_STR);
            }

            // читаем значение АЦП
            std::ifstream in_adc(ADC_STR.c_str());
            if (in_adc.is_open()) {
                in_adc >> adc;
                in_adc.close();
            } else {
                /*
                 * файла нет
                 */
                LOG_ERROR("Device %s not found. Voltage set by zero", ADC_STR);
                adc = 0;
            }
#endif
            _voltage.store(adc * adc_config);
            LOG_DEBUG("End loop measuring voltage");
            Poco::Thread::sleep(60000);
        } catch (...) {
            LOG_ERROR("Error in loop measuring voltage");
        }
    }
}

float Voltage::getVoltage() {
    return _voltage.load();
}
