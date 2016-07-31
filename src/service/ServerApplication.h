//
// Created by madrus on 24.07.16.
//

#ifndef PSERVER_SERVERAPPLICATION_H
#define PSERVER_SERVERAPPLICATION_H


#include <Poco/Util/ServerApplication.h>

class ServerApplication : public Poco::Util::ServerApplication {
public:
    ServerApplication();

    virtual ~ServerApplication();
protected:

    int main(const std::vector<std::string>& args);

    /**
     * Start application
     *
     * @return true if successful, false otherwise
     */
    bool start();

    /**
     * Stop application
     */
    void stop();

    void displayHelp();

private:
    Poco::Logger& _logger;
    bool _helpRequested;
    Poco::Thread _threadVoltage;
};


#endif //PSERVER_SERVERAPPLICATION_H
