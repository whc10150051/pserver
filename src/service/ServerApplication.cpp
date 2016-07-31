//
// Created by madrus on 24.07.16.
//

#include "Server.h"
#include "Voltage.h"
#include "ServerApplication.h"
#include "ServiceName.h"
#include "ProductInfo.h"
#include <iostream>
#include <Poco/Net/TCPServerParams.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Process.h>
#include <Poco/Environment.h>

ServerApplication::ServerApplication() : _logger(Poco::Logger::get("")) {

}

ServerApplication::~ServerApplication() {

}

int ServerApplication::main(const std::vector<std::string>& args) {
    if (_helpRequested) {
        displayHelp();
        return Poco::Util::Application::EXIT_OK;
    }

    if (!start()) {
        return Poco::Util::Application::EXIT_SOFTWARE;
    }

    // wait for CTRL-C or kill
    waitForTerminationRequest();

    stop();
    return Poco::Util::Application::EXIT_OK;
}

void ServerApplication::displayHelp() {
    Poco::Util::HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.format(std::cout);
}

bool ServerApplication::start() {
    LOG_INFO("Environment:\n\t"
             "OS Name: %s\n\t"
             "OS Version: %s\n\t"
             "OS Arch: %s\n\t"
             "Node Name: %s\n\t"
             "Node ID: %s",
            Poco::Environment::osName(),
            Poco::Environment::osVersion(),
            Poco::Environment::osArchitecture(),
            Poco::Environment::nodeName(),
            Poco::Environment::nodeId());
    LOG_INFO("Service initialization");


    LOG_INFO("Service initialization complete. Product version: %s", std::string(RES_PRODUCT_VERSION));

    if (!config().hasOption("service.name")) {
        std::string serviceName = getServiceName(Poco::Process::id());
        config().setString("service.name", serviceName);
    }
    LOG_INFO("Service Name: %s", Poco::Util::Application::instance().config().getString("service.name"));

    try {
        Poco::AutoPtr<Poco::Net::TCPServerParams> params = new Poco::Net::TCPServerParams();
        params->setMaxThreads(4);
        params->setMaxQueued(4);
        params->setThreadIdleTime(100);

        Server(5102, params);

        Voltage voltage;
        _threadVoltage.start(voltage);
    }
    catch (const Poco::Exception& ex) {
        LOG_FATAL("Failed to start modules: %s", std::string(ex.displayText()));
        stop();
        return false;
    }
    catch (const std::exception& ex) {
        LOG_FATAL("Failed to start modules: %s", std::string(ex.what()));
        stop();
        return false;
    }
    catch (...) {
        LOG_FATAL("Unknown error when starting modules");
        stop();
        return false;
    }
    return true;
}

void ServerApplication::stop() {
    LOG_DEBUG("Wait for the completion of the thread");
    _threadVoltage.join();
    LOG_DEBUG("Thread has been stopped");
}