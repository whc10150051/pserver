#include <iostream>
#include <src/Server.h>
#include <memory>
#include <ctime>
#include <Poco/JSON/Object.h>
#include <src/Connection.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <src/utils/StringHelper.h>
#include "Poco/Timestamp.h"

// посылка объекта
void send(Poco::Logger& _logger, Poco::Net::StreamSocket& ss, Poco::JSON::Object::Ptr object) {
    std::string str = StringHelper::objectToString(object);
    LOG_DEBUG("request : %s", str);
    LOG_DEBUG("request size: %d", (int) str.size());
    ss.sendBytes(str.data(), (int) str.size());
}

// тупой прием и отображение
std::string receive(Poco::Logger& _logger, Poco::Net::StreamSocket& ss) {
    Poco::Timestamp begin;
    Poco::Timespan timeOut(60000,0); // ждем минуту (поместить в константы)
    std::string answer;

    // Эмуляция измерения, ждем
    if (ss.poll(timeOut,Poco::Net::Socket::SELECT_READ) == false){
        LOG_DEBUG("TIMEOUT! exit");
    } else {
        while (ss.available() > 0) {
            char buffer[2048] = {0};

//            LOG_DEBUG("RX EVENT!!! ---> ");
            int nBytes = -1;

            try {
                LOG_DEBUG("Available bytes: %d", ss.available());
                nBytes = ss.receiveBytes(buffer, sizeof(buffer));
            } catch (Poco::Exception& e) {
                //Handle your network errors.
                LOG_ERROR("Network error: %s", e.displayText());
                break;
            }
            answer += std::string(buffer, nBytes);
            if (nBytes == 0) {
                LOG_DEBUG("Client closes connection!");
                break;
            }
            else {
                LOG_DEBUG("Receiving nBytes: %d", nBytes);
            }
        }
    }
    // если большие данные. то не печатать
    LOG_DEBUG("receive: %s", answer);
    LOG_DEBUG("receive size: %d byte", (int) answer.size());

    Poco::Timestamp::TimeDiff diff = Poco::Timestamp() - begin;
    LOG_DEBUG("receive time: %d ms", (int)(diff / 1000));

    return answer;
}

int main() {
// запуск как демона будет таким
//    class Daemon : public ServerApplication
//    {
//    public:
//        Daemon();
//        /// @Brief The main loop of the daemon, everything must take place here
//        int main();
//    };
//    int Daemon::main()
//    {
//        Poco::Net::TCPServerParams* params = new Poco::Net::TCPServerParams();
//        params->setMaxThreads(4); // в конфиг
//        params->setMaxQueued(4);// в конфиг
//        params->setThreadIdleTime(100);// в конфиг
//
//        Server(5102, params); // в конфиг
//        // Wait for CTRL+C
//        waitForTerminationRequest();
//        return Application::EXIT_OK;
//    }


    // init log
    Poco::AutoPtr<Poco::PatternFormatter> formatter(new Poco::PatternFormatter());
    formatter->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: [%p] %t");
    formatter->setProperty("times", "local");

    Poco::AutoPtr<Poco::FormattingChannel> consoleChannel(
            new Poco::FormattingChannel(formatter, new Poco::ConsoleChannel()));

    Poco::Logger::root().setLevel(Poco::Message::Priority::PRIO_TRACE);
    Poco::Logger::root().setChannel(consoleChannel);

    Poco::Logger& _logger(Poco::Logger::get("Main"));

    Poco::Net::TCPServerParams* params = new Poco::Net::TCPServerParams();
    params->setMaxThreads(4);
    params->setMaxQueued(4);
    params->setThreadIdleTime(100);

    Server srv(5102, params);



    // Client
    // Эмуляция запуска 2х команд последовательно и постоянного опроса статуса
    class Status: public Poco::Runnable {
    public:
        Status() : Poco::Runnable(), _logger(Poco::Logger::get("Status")) {
        }

    private:
        virtual void run() {
            int n = 10;
            while (n--) {
                Poco::Net::SocketAddress sa("localhost", 5102);
                Poco::Net::StreamSocket ss(sa);
                // create task Fitting
                Poco::JSON::Object::Ptr status = new Poco::JSON::Object(true);
                status->set("task", "status");

                // send
                send(_logger, ss, status);

                // receive
                LOG_DEBUG("status %s", receive(_logger, ss));

                Poco::Thread::sleep(100);
                // close
                ss.close();

                Poco::Thread::sleep(1000);
            }
        }

    private:
        Poco::Logger& _logger;
    };

    // опрос статуса
    Status runnable;
    Poco::Thread threadStatus;
    threadStatus.start(runnable);

#if 0
    // запуск задачи по притирке
    {
        Poco::Net::SocketAddress sa("localhost", srv.socket().address().port());
        Poco::Net::StreamSocket ss(sa);
        // create task Fitting
        Poco::JSON::Object::Ptr fitting = new Poco::JSON::Object(true);
        fitting->set("task", "fitting");
        fitting->set("sensishi", 0);
        fitting->set("voltishi", 0);
        fitting->set("channel", 0);
        fitting->set("relay", 0);
        fitting->set("tau", 50); // millsec
        fitting->set("l", 1);
        fitting->set("m", 10);
        fitting->set("nstart", 2100);
        fitting->set("lpost", 200000);
        fitting->set("size", 20000);

        // send
        send(_logger, ss, fitting);

        // receive
        receive(_logger, ss);

        Poco::Thread::sleep(100);
        // close
        ss.close();
    }
    Poco::Thread::sleep(10000);
#endif

#if 0
    // эхо
    {
    // create task Echo
        Poco::Net::SocketAddress sa("localhost", srv.socket().address().port());
        Poco::Net::StreamSocket ss(sa);
        Poco::JSON::Object::Ptr echo = new Poco::JSON::Object(true);
        echo->set("task", "echo");
        echo->set("data", "123456789qwerty");

        // send
        send(_logger, ss, echo);

        // receive
        receive(_logger, ss);

        Poco::Thread::sleep(100);
        // close
        ss.close();
    }
    sleep(1);
#endif


    // запуск задачи по почастотке
    {
        Poco::Net::SocketAddress sa("localhost", srv.socket().address().port());
        Poco::Net::StreamSocket ss(sa);
        // create task ScanResonanceTask
        Poco::JSON::Object::Ptr scan = new Poco::JSON::Object(true);
        scan->set("task", "scan");
        scan->set("relay", 0);
        scan->set("channel", 0);
        scan->set("sensishi", 0);
        scan->set("voltishi", 0);
        scan->set("mode", 0);
        scan->set("n", 0);
        scan->set("r", 0);
        scan->set("nstart", 2100);
        scan->set("lpost", 200000);
        scan->set("scansteps", 12);
        // это тест передачи многокомпонентного сигнала
        size_t numComponent = 5;
        Poco::JSON::Array::Ptr componentArray = new Poco::JSON::Array();
        for (size_t idx = 0; idx < numComponent; ++idx) {
            Poco::JSON::Object::Ptr component = new Poco::JSON::Object(true);
            component->set("m", std::to_string(10));
            component->set("l", std::to_string(1));
            component->set("tau", std::to_string(50)); // millsec
            component->set("mfo", std::to_string(50));
            componentArray->set(idx, component);
        }
        scan->set("probingsignal", componentArray);


        // send
        send(_logger, ss, scan);

        // receive
        receive(_logger, ss);

        Poco::Thread::sleep(100);
        // close
        ss.close();
    }
    Poco::Thread::sleep(10000);


    threadStatus.join();
    return 0;
}