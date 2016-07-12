//
// Created by madrus on 26.06.16.
//

#include "Server.h"
#include "Connection.h"
#include <Poco/SyslogChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Path.h>

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
        // перенести в  конфиг
        const Poco::Path &logDirectory = "../log";
        std::string logFileName = "pserver.log";
        Poco::Path logPath(logDirectory, logFileName);

        Poco::AutoPtr<Poco::PatternFormatter> formatter(new Poco::PatternFormatter());
        formatter->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: [%p] %t");
        formatter->setProperty("times", "local");

        Poco::AutoPtr<Poco::FormattingChannel> consoleChannel(
                new Poco::FormattingChannel(formatter, new Poco::ConsoleChannel()));

//    formatter->setProperty("rotation", "2 K");
//    formatter->setProperty("archive", "timestamp");

//        Poco::AutoPtr<Poco::FormattingChannel> fileChannel = new Poco::FormattingChannel(formatter,
//                                                                                         new Poco::FileChannel(
//                                                                                                 logPath.toString()));

        Poco::AutoPtr<Poco::SplitterChannel> splitter(new Poco::SplitterChannel);
        splitter->addChannel(consoleChannel);
//        splitter->addChannel(fileChannel);

        Poco::Logger::root().setLevel(Poco::Message::Priority::PRIO_TRACE);
        Poco::Logger::root().setChannel(splitter);
    } catch (Poco::Exception& ex) {
        std::cerr << "Server::configureLogger: " << ex.displayText() << std::endl;
    }
}
