//
// Created by madrus on 26.06.16.
//

#ifndef PSERVER_MEASURECONNECTION_H
#define PSERVER_MEASURECONNECTION_H

#include <sys/socket.h>
#include <iostream>
#include <Poco/Net/TCPServerConnection.h>
#include <src/utils/Logger.h>

class MeasureConnection: public Poco::Net::TCPServerConnection {
public:
    MeasureConnection(const Poco::Net::StreamSocket& s): TCPServerConnection(s), _logger(Poco::Logger::get("MeasureConnection")) { }

    void run();

private:
    Poco::Logger& _logger;
};

#endif //PSERVER_DATACONNECTION_H
