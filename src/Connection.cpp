//
// Created by madrus on 26.06.16.
//

#include "Connection.h"
#include "tasks/TaskFactory.h"
#include "Server.h"
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <src/utils/StringHelper.h>

void Connection::run() {
    LOG_DEBUG("Begin connection");
    AbstractTask::Ptr task = nullptr;
    try {
        std::string request = receive();
        if (!request.empty()) {
            Poco::Dynamic::Var dataVar = Poco::JSON::Parser().parse(request);
            Poco::JSON::Object::Ptr config = dataVar.extract<Poco::JSON::Object::Ptr>();

            task = TaskFactory::create(config);
            if (task->run()) {
                send(StringHelper::objectToString(task->getAnswer()));
            }
        } else {
            LOG_ERROR("request is empty");
        }
    } catch (Poco::Exception& ex) {
        LOG_ERROR("run: %s", ex.displayText());

        std::string name = "unknown";
        if (task) {
            name = task->getName();
        }
        try {
            Poco::JSON::Object::Ptr answer = new Poco::JSON::Object(true);
            answer->set("name", name);
            answer->set("status", "error");
            answer->set("code", ex.displayText());
            send(StringHelper::objectToString(answer));
        } catch (Poco::Exception &ex_) {
            LOG_ERROR("Can not send error answer from Connection::run() : %s ( error answer: %s)",
                      ex_.displayText(), ex.displayText());
        }
    }
    LOG_DEBUG("End connection");
}

void Connection::send(const std::string& text) {
    std::lock_guard<std::mutex> lock(_sendMutex);
    Poco::Net::StreamSocket& ss = socket();
    int n = text.size();
    LOG_DEBUG("size answer: %d byte", n);
    const char* answer = text.c_str();
//                LOG_DEBUG("answer: %s", answer);

    while (n > 0) {
        ss.sendBytes(answer, n);
        n = ss.receiveBytes((void*)answer, sizeof(answer));
    }
}

std::string Connection::receive() {
    std::lock_guard<std::mutex> lock(_receiveMutex);
    Poco::Net::StreamSocket& ss = socket();
    Poco::Timespan timeOut(10000, 0); // занести в конфиг
    std::string request;

    if (ss.poll(timeOut, Poco::Net::Socket::SELECT_READ) == false) {
        LOG_DEBUG("TIMEOUT! %d ms - exit", timeOut.milliseconds());
    } else {
        while (ss.available() > 0) {
            char buffer[1024] = {0};
            int n = ss.receiveBytes(buffer, sizeof(buffer));
            request += std::string(buffer, n);
        }
        LOG_DEBUG("request size : %d byte", (int) request.size());
        LOG_DEBUG("request : %s", request);
    }
    return request;
}
