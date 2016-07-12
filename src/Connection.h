//
// Created by madrus on 26.06.16.
//

#pragma once

#include <sys/socket.h>
#include <iostream>
#include <Poco/Net/TCPServerConnection.h>
#include <src/utils/Logger.h>
#include <mutex>

class Connection: public Poco::Net::TCPServerConnection {
public:
    Connection(const Poco::Net::StreamSocket& s): TCPServerConnection(s), _logger(Poco::Logger::get("Connection")) { }

    void run();

private:
    void send(const std::string& text);
    std::string receive();

private:
    Poco::Logger& _logger;
    std::mutex _sendMutex;
    std::mutex _receiveMutex;
};
