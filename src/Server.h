//
// Created by madrus on 26.06.16.
//
#pragma once

#include "utils/Logger.h"
#include <atomic>
#include <memory>
#include <Poco/Net/TCPServer.h>

/**
 * Многопоточный сервер, имеет несколько соединений
 * 1. Служебный - по таймеру обмнивается с клиентом состоянием сервера (постоянный)
 * 2. Измерительный - проведение измерений (может закрываться)
 */
class Server : public Poco::Net::TCPServer {
public:
    Server(int port, Poco::Net::TCPServerParams* params);
    ~Server();
    void configureLogger();

private:
    Poco::Logger& _logger;
};

class Status {
public:

    static std::string getStatus();
    static void setStatus(const std::string &_status);
    static void clear();

protected:
    static Status* Instance() {
        if (!_self) {
            _self = new Status();
        }
        return _self;
    }
    static Status* _self;
    Status() {}
    virtual ~Status() {}

private:
    std::string _status;
};