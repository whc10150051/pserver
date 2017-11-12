//
// Created by madrus on 26.06.16.
//
#pragma once

#include "utils/Logger.h"
#include <atomic>
#include <memory>
#include <Poco/Net/TCPServer.h>
#include <Poco/FormattingChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/PatternFormatter.h>

/**
 * Многопоточный сервер, имеет несколько соединений (каналов)
 * 1. Служебный канал - по переодически (по таймеру) обменивается с клиентом состоянием сервера (постоянный)
 * 2. Измерительный канал - проведение измерений (может закрываться)
 */
class Server : public Poco::Net::TCPServer {
public:
    Server(int port, Poco::Net::TCPServerParams* params);
    ~Server() override;
    void configureLogger();

private:
    Poco::Logger& _logger;
    Poco::AutoPtr<Poco::PatternFormatter> _formatter;
    Poco::AutoPtr<Poco::FileChannel> _fileChannel;
    Poco::AutoPtr<Poco::FormattingChannel> _channel;
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
