#include <iostream>
#include <src/Server.h>
#include <memory>
#include <Poco/JSON/Object.h>
#include <src/MeasureConnection.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>

using namespace std;

int main() {
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
    Poco::Net::SocketAddress sa("localhost", srv.socket().address().port());
    Poco::Net::StreamSocket ss(sa);

    // create task
    Poco::JSON::Object::Ptr object = new Poco::JSON::Object(true);
    object->set("task", "fitting");
    object->set("sensishi", 0);
    object->set("voltishi", 0);
    object->set("channel", 0);
    object->set("relay", 0);
    object->set("tau", 50); // millsec
    object->set("l", 1);
    object->set("m", 10);
    object->set("nstart", 2100);
    object->set("lpost", 200000);
    object->set("size", 20000);

    std::ostringstream os;
    object->stringify(os);

    LOG_DEBUG("request : %s", os.str());

    ss.sendBytes(os.str().data(), (int) os.str().size());
    char buffer[2048] = {0};
    int n = ss.receiveBytes(buffer, sizeof(buffer));
    LOG_DEBUG("receive: %s", std::string(buffer, n));

    return 0;
}