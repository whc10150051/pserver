//
// Created by madrus on 26.06.16.
//

#include "Server.h"
#include "Connection.h"
#include <Poco/SyslogChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/SimpleFileChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Path.h>

const Poco::Path LOG_DIRECTORY = "/var/log/";
const std::string LOG_FILENAME = "pserver.log";

Server::Server(int port, Poco::Net::TCPServerParams* params) :
        TCPServer(new Poco::Net::TCPServerConnectionFactoryImpl<Connection>(),
        Poco::Net::ServerSocket(port), params), _logger(Poco::Logger::get("Server")) {
    configureLogger();
    LOG_DEBUG("Server start");
    start();
    LOG_DEBUG("Server start successfully");
}

Server::~Server() {
    LOG_DEBUG("Server stop");
    stop();
    LOG_DEBUG("Server is stopped");
}

void Server::configureLogger() {
    try {
        Poco::Path logPath(LOG_DIRECTORY, LOG_FILENAME);
        std::cout << "Path logger: " << logPath.toString() << std::endl;

        try {
            _formatter = new Poco::PatternFormatter();
            _formatter->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: [%p] %t");
            _formatter->setProperty("times", "local");
        } catch (const Poco::Exception& ex) {
            std::cerr << "Server configure logger failed (Text mode)" << std::endl;
            throw;
        }

        try {
            _fileChannel = new Poco::FileChannel();
            _fileChannel->setProperty("path", logPath.toString());
            _fileChannel->setProperty("rotation", "20M");
            _fileChannel->setProperty("compress", "true");
            _fileChannel->setProperty("archive", "timestamp");
            _fileChannel->setProperty("purgeCount", "20");
        } catch (const Poco::Exception& ex) {
            std::cerr << "Server configure logger failed (File mode)" << std::endl;
            throw;
        }

        _channel = new Poco::FormattingChannel(_formatter.get(), _fileChannel.get());
        _logger.setLevel(Poco::Message::Priority::PRIO_TRACE);
        _logger.setChannel(_channel.get());

    } catch (const Poco::Exception& ex) {
        std::cerr << "Server configure logger failed: " << ex.displayText() << std::endl;
    }
}

std::string Status::getStatus() {
    return Instance()->_status;
}

void Status::setStatus(const std::string& status) {
    Instance()->_status = status;
}

void Status::clear() {
    Status::setStatus("");
}

Status* Status::_self = nullptr;
