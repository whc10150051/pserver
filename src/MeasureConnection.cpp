//
// Created by madrus on 26.06.16.
//

#include "MeasureConnection.h"
#include "tasks/TaskFactory.h"
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

void MeasureConnection::run() {
    Poco::Net::StreamSocket& ss = socket();
    try {
        Poco::FIFOBuffer buffer(1024);
        int n = ss.receiveBytes(buffer, sizeof(buffer));

        LOG_DEBUG("request : %s", std::string(buffer));

        /**
         * JSON запрос
         */
        Poco::JSON::Parser jsonParser;
        Poco::Dynamic::Var dataVar = jsonParser.parse(buffer);
        Poco::JSON::Object::Ptr config = dataVar.extract<Poco::JSON::Object::Ptr>();

        AbstractTask::Ptr task = nullptr;

        try {
            task = TaskFactory::create(config);
            if (task->run()) {
                Poco::JSON::Object::Ptr answer = task->getAnswer();
                std::ostringstream os;
                answer->stringify(os);
                std::string answerStr = os.str();
                n = answerStr.size();
                const char* answerBuffer = answerStr.c_str();
                LOG_DEBUG("answerBuffer: %s", answerStr);

                while (n > 0) {
                    ss.sendBytes(answerBuffer, n);
                    n = ss.receiveBytes((void*)answerBuffer, sizeof(answerBuffer));
                }
            }
        } catch (Poco::Exception& ex) {
            LOG_DEBUG("MeasureConnection : %s", ex.displayText());
        }
    }
    catch (Poco::Exception& exc) {
        LOG_DEBUG("MeasureConnection: %s", exc.displayText());
    }
}
