//
// Created by madrus on 24.07.16.
//

#ifndef PSERVER_SERVERAPPLICATION_H
#define PSERVER_SERVERAPPLICATION_H

#include <Poco/Util/ServerApplication.h>

/**
 * maximum number of simultaneous threads available for this TCPServerDispatcher
 */
#define THREAD_NUMBER 4
/**
 * sets the maximum number of queued connections
 */
#define QUEUE_CONNECTION_NUMBER 4
/**
 * maximum idle time for a thread before it is terminated
 */
#define MAX_IDLE_TIME 100
/**
 * port connection with server
 */
#define SERVER_PORT 5102


/**
 * Service application
 *
 * Instance of this class is created in main() function.
 */
class ServiceApplication : public Poco::Util::ServerApplication {
public:
    ServiceApplication();

    virtual ~ServiceApplication();
protected:

    int main(const std::vector<std::string>& args);

    /**
     * Start application
     *
     * @return true if successful, false otherwise
     */
    bool start();
    /**
     * Initialize the application and all registered subsystems
     *
     * @param self self
     */
    void initialize(Poco::Util::Application& self);

    /**
     * Uninitialize the application and all registered subsystems
     */
    void uninitialize();

    /**
     * Define supported command line arguments
     *
     * @param options options
     */
    void defineOptions(Poco::Util::OptionSet& options);

    /**
     * Handle command line argument
     *
     * @param name argument name
     * @param value argument value
     */
    void handleOption(const std::string& name, const std::string& value);

    /**
     * Handle help option
     *
     * @param name argument name
     * @param value argument value
     */
    void handleHelp(const std::string& name, const std::string& value);
    /**
     * Stop application
     */
    void stop();

    void displayHelp();

private:
    Poco::Logger& _logger;
    bool _helpRequested;
    std::string _serviceName; ///< service name
    Poco::Thread _threadVoltage;
};


#endif //PSERVER_SERVERAPPLICATION_H
