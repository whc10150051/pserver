//
// Created by madrus on 26.06.16.
//

#ifndef PSERVER_CONST_H
#define PSERVER_CONST_H

#include <string>

/*
 * лог работы демона
 */
const std::string LOG_STR = "/var/log/mserver.log";
/*
 * конфигурационный файл АЦП
 */
const std::string ADC_CONF_STR = "/usr/local/www/adc.conf";
/*
 * файл значения АЦП
 */
const std::string ADC_STR = "/sys/devices/ocp.3/44e0d000.tscadc/tiadc/iio:device0/in_voltage6_raw";
/*
 * GPIO c признаком того, что прибор подключен к сети и заряжается
 */
const std::string GPIO_CHARGING = "/sys/class/gpio/gpio7/value";
/*
 * если файл есть то загрузились с eMMC в режиме восстановления
 */
const std::string FILE_RECOVERY = "/usr/local/www/IS_RECOVERY";

#endif //PSERVER_CONST_H
