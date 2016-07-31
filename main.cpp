#include "Server.h"
#include "ServerApplication.h"
#include "Connection.h"
#include "StringHelper.h"
#include <Poco/JSON/Object.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Timestamp.h>
#include <Poco/Util/ServerApplication.h>
#include <memory>
#include <ctime>
#include <src/Voltage.h>
#include <iostream>
#include <sys/time.h>
#include <Poco/Random.h>

//POCO_SERVER_MAIN(ServerApplication);

#if 1
// посылка объекта
void send(Poco::Logger& _logger, Poco::Net::StreamSocket& ss, Poco::JSON::Object::Ptr object) {
    std::string str = StringHelper::objectToString(object);
    LOG_DEBUG("request : %s", str);
    LOG_DEBUG("request size: %d", (int) str.size());
    ss.sendBytes(str.data(), (int) str.size());
}

// тупой прием и отображение
std::string receive(Poco::Logger& _logger, Poco::Net::StreamSocket& ss, bool isBig = false) {
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
    if (!isBig) {
        LOG_DEBUG("receive: %s", answer);
    }
    LOG_DEBUG("receive size: %d byte", (int) answer.size());

    Poco::Timestamp::TimeDiff diff = Poco::Timestamp() - begin;
    LOG_DEBUG("receive time: %d ms", (int)(diff / 1000));

    return answer;
}

//#define TEST
//#define TEST_CONVERT

long timer_msec() {
    timeval tv;
    gettimeofday (&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
    Poco::AutoPtr<Poco::Net::TCPServerParams> params = new Poco::Net::TCPServerParams();
    params->setMaxThreads(4);
    params->setMaxQueued(4);
    params->setThreadIdleTime(100);

    Server srv(5102, params);

    Voltage voltage;
    Poco::Thread threadVoltage;
    threadVoltage.start(voltage);

#ifdef TEST
    Poco::Logger& _logger(Poco::Logger::get(""));
#endif


#ifdef TEST_CONVERT
    Poco::Logger& _logger(Poco::Logger::get(""));


    long time = timer_msec();
    Poco::Random rnd;
    rnd.seed();

    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object(true);
    Poco::JSON::Array::Ptr array = new Poco::JSON::Array();
    for (size_t idx = 0; idx < 250000; ++idx) {
        Poco::JSON::Object::Ptr component = new Poco::JSON::Object(true);
        array->add(std::to_string(rnd.next(999999999)));
    }
    obj->set("pagc", array);

    LOG_DEBUG("time %d", (int)(timer_msec() - time));


#endif

// опрос статуса
#ifdef TEST
    // Client
    // Эмуляция запуска 2х команд последовательно и постоянного опроса статуса
    class Status: public Poco::Runnable {
    public:
        Status() : Poco::Runnable(), _logger(Poco::Logger::get("Status")) {
        }

    private:
        virtual void run() {
            int n = 1000;
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

                Poco::Thread::sleep(100);
            }
        }

    private:
        Poco::Logger& _logger;
    };

    Status runnable;
    Poco::Thread threadStatus;
    threadStatus.start(runnable);
#endif

// запуск задачи по притирке
#ifdef TEST
    {
        Poco::Net::SocketAddress sa("localhost", srv.socket().address().port());
        Poco::Net::StreamSocket ss(sa);
        // create task Fitting
        Poco::JSON::Object::Ptr fitting = new Poco::JSON::Object(true);
        fitting->set("task", "fitting");
        fitting->set("channel", 0);
        fitting->set("relay", 0);
        fitting->set("sensishi", 0);
        fitting->set("voltishi", 0);
        fitting->set("nstart", 2100);
        fitting->set("lpost", 200000);
        fitting->set("mode", 1);
        fitting->set("n", 1);
        fitting->set("r", 2);
        fitting->set("m", 10);
        fitting->set("l", 1);
        fitting->set("tau", "502.465"); // millsec
        fitting->set("mfo", "0.0050"); // millsec
        fitting->set("size", 20000);

        // send
        send(_logger, ss, fitting);

        Poco::Thread::sleep(2000); //

        // receive
        receive(_logger, ss, true);

        Poco::Thread::sleep(100);
        // close
        ss.close();
    }
    Poco::Thread::sleep(10000);
#endif

// эхо
#ifdef TEST
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
#ifdef TEST
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
        size_t pagcSize = 1;
        Poco::JSON::Array::Ptr pagcArray = new Poco::JSON::Array();
        for (size_t idx = 0; idx < pagcSize; ++idx) {
            Poco::JSON::Object::Ptr component = new Poco::JSON::Object(true);
            pagcArray->add(std::to_string(10));
        }
        scan->set("pagc", pagcArray);

        scan->set("scansteps", 12);
        // это тест передачи многокомпонентного сигнала
        size_t numComponent = 5;
        Poco::JSON::Array::Ptr componentArray = new Poco::JSON::Array();
        for (size_t idx = 0; idx < numComponent; ++idx) {
            Poco::JSON::Object::Ptr component = new Poco::JSON::Object(true);
            component->set("m", 10);
            component->set("l", 1);
            component->set("tau", std::to_string(50)); // millsec
            component->set("mfo", std::to_string(50));
            componentArray->set(idx, component);
        }
        scan->set("probingsignal", componentArray);


        // send
        send(_logger, ss, scan);

        // receive
        receive(_logger, ss, true);

        Poco::Thread::sleep(100);
        // close
        ss.close();
    }
    Poco::Thread::sleep(10000);
#endif

// запуск измерения
#ifdef TEST
    {
        Poco::Net::SocketAddress sa("localhost", srv.socket().address().port());
        Poco::Net::StreamSocket ss(sa);
        // create task MeasureTask
        Poco::JSON::Object::Ptr measure = new Poco::JSON::Object(true);
        measure->set("task", "measure");
        measure->set("relay", 0);
        measure->set("channel", 0);
        measure->set("sensishi", 0);
        measure->set("voltishi", 0);
        measure->set("mode", 0);
        measure->set("n", 0);
        measure->set("r", 0);
        measure->set("nstart", 2100);
        measure->set("lpost", 200000);
        size_t pagcSize = 1;
        Poco::JSON::Array::Ptr pagcArray = new Poco::JSON::Array();
        for (size_t idx = 0; idx < pagcSize; ++idx) {
            Poco::JSON::Object::Ptr component = new Poco::JSON::Object(true);
            pagcArray->add(std::to_string(10));
        }
        measure->set("pagc", pagcArray);
        // это тест передачи многокомпонентного сигнала
        size_t numComponent = 5;
        Poco::JSON::Array::Ptr componentArray = new Poco::JSON::Array();
        for (size_t idx = 0; idx < numComponent; ++idx) {
            Poco::JSON::Object::Ptr component = new Poco::JSON::Object(true);
            component->set("m", 10);
            component->set("l", 1);
            component->set("tau", std::to_string(50)); // millsec
            component->set("mfo", std::to_string(50));
            componentArray->set(idx, component);
        }
        measure->set("probingsignal", componentArray);
        measure->set("typeSignal", "env_afc");


        // send
        send(_logger, ss, measure);

        // receive
        receive(_logger, ss, true);

        Poco::Thread::sleep(100);
        // close
        ss.close();
    }
    Poco::Thread::sleep(10000);
#endif

// запуск универсальный таск
#ifdef TEST
    {
        // create task Task
        std::vector<std::string> tasks = {"getNumСhannels", "disableMu", "getTemp", "shutdown"};
        for (auto& name : tasks) {
            Poco::Net::SocketAddress sa("localhost", srv.socket().address().port());
            Poco::Net::StreamSocket ss(sa);
            Poco::JSON::Object::Ptr universal = new Poco::JSON::Object(true);
            universal->set("task", "universal");
            universal->set("type", name);
            if (name == "getTemp") {
                universal->set("data", "2");
            }

            // send
            send(_logger, ss, universal);

            // receive
            receive(_logger, ss);

            Poco::Thread::sleep(100);
            // close
            ss.close();
        }
    }
#endif

#ifdef TEST
    threadVoltage.join();
    threadStatus.join();
#endif

#ifndef TEST
    while (1);
#endif

    return 0;
}

#endif // if 0