//
// Created by madrus on 24.07.16.
//

#include "Server.h"
#include "Voltage.h"
#include "ServiceApplication.h"
#include "ServiceName.h"
#include "ProductInfo.h"
#include <iostream>
#include <Poco/Net/TCPServerParams.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Process.h>
#include <Poco/Environment.h>

ServiceApplication::ServiceApplication() :
        _logger(Poco::Logger::get("")),
        _helpRequested(false) {

}

ServiceApplication::~ServiceApplication() {

}

void ServiceApplication::initialize(Application& self) {
    if (_helpRequested) {
        return;
    }

    LOG_INFO("Service initialization");
    Poco::Util::ServerApplication::initialize(self);
    LOG_INFO("Service initialization complete");
}

void ServiceApplication::uninitialize() {
    if (_helpRequested) {
        return;
    }

    LOG_INFO("Service uninitialization");
    Poco::Util::ServerApplication::uninitialize();
    LOG_INFO("Service uninitialization complete");
}

int ServiceApplication::main(const std::vector<std::string>& args) {
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

void ServiceApplication::defineOptions(Poco::Util::OptionSet& options) {
    Poco::Util::ServerApplication::defineOptions(options);

    options.addOption(
            Poco::Util::Option("help", "h", "Display help information")
                    .required(false)
                    .repeatable(false)
                    .callback(Poco::Util::OptionCallback<ServiceApplication>(this, &ServiceApplication::handleHelp)));
}

void ServiceApplication::handleOption(const std::string& name,
                                      const std::string& value) {
    Poco::Util::ServerApplication::handleOption(name, value);
    if (name == "serviceName") {
        _serviceName = value;
    }
}

void ServiceApplication::handleHelp(const std::string& name, const std::string& value) {
    _helpRequested = true;
    stopOptionsProcessing();
}

void ServiceApplication::displayHelp() {
    Poco::Util::HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.format(std::cout);
}

bool ServiceApplication::start() {

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
        LOG_INFO("Max number threads: %d", THREAD_NUMBER);
        params->setMaxThreads(THREAD_NUMBER);
        LOG_INFO("Max number queued: %d", QUEUE_CONNECTION_NUMBER);
        params->setMaxQueued(QUEUE_CONNECTION_NUMBER);
        LOG_INFO("Max idle time: %d", MAX_IDLE_TIME);
        params->setThreadIdleTime(MAX_IDLE_TIME);

        LOG_INFO("Server port: %d", SERVER_PORT);
        Server srv(SERVER_PORT, params);
        usleep(1000);

        LOG_INFO("Start voltage status thread");
        Voltage voltage;
        Poco::Thread threadVoltage;
        threadVoltage.start(voltage);
        LOG_INFO("Server is running");
        usleep(1000);

    } catch (const Poco::Exception& ex) {
        LOG_FATAL("Failed to start modules: %s", std::string(ex.displayText()));
        stop();
        return false;
    } catch (const std::exception& ex) {
        LOG_FATAL("Failed to start modules: %s", std::string(ex.what()));
        stop();
        return false;
    } catch (...) {
        LOG_FATAL("Unknown error when starting modules");
        stop();
        return false;
    }
    return true;
}

void ServiceApplication::stop() {
    LOG_DEBUG("Wait for the completion of the thread");
    _threadVoltage.join();
    LOG_DEBUG("Thread has been stopped");
}