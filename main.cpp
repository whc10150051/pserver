#include <src/Voltage.h>
#include "Server.h"
#include "ServiceApplication.h"

/**
 * Run service
 */
// пока этот вариант не работает (не запускается сервет, порт не открывается)
//int main(int argc, char *argv[]) {
//    ServiceApplication app;
//    return app.run(argc, argv);
//}

int main(int argc, char *argv[]) {
    Poco::AutoPtr<Poco::Net::TCPServerParams> params = new Poco::Net::TCPServerParams();
    params->setMaxThreads(4);
    params->setMaxQueued(4);
    params->setThreadIdleTime(100);

    Server srv(5102, params);

    Voltage voltage;
    Poco::Thread threadVoltage;
    threadVoltage.start(voltage);

    while(1) {
        sleep(10);
    }
}
