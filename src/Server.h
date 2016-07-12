//
// Created by madrus on 26.06.16.
//
#pragma once

#include "utils/Logger.h"
#include <Poco/Net/TCPServer.h>
#include <mutex>

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
