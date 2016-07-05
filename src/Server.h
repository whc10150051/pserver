//
// Created by madrus on 26.06.16.
//

#ifndef PSERVER_SERVER_H
#define PSERVER_SERVER_H

#include "utils/Logger.h"
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

#endif //PSERVER_SERVER_H
