//
// Created by madrus on 24.07.16.
//

#ifndef PSERVER_VOLTAGE_H
#define PSERVER_VOLTAGE_H

#include "Logger.h"
#include <memory>
#include <Poco/Runnable.h>
#include <Poco/Logger.h>
#include <Poco/Thread.h>
#include <atomic>

class Voltage: public Poco::Runnable {
public:
    Voltage() : Poco::Runnable(), _logger(Poco::Logger::get("Voltage")) {
    }

    float getVoltage();

private:
    virtual void run();

private:
    Poco::Logger& _logger;
    std::atomic<float> _voltage;
};

#endif //PSERVER_VOLTAGE_H
