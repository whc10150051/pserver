//
// Created by madrus on 26.06.16.
//

#include "EchoConnection.h"

void EchoConnection::run() {
    Poco::Net::StreamSocket& ss = socket();
    try {
        char buffer[256];
        int n = ss.receiveBytes(buffer, sizeof(buffer));
        while (n > 0) {
            ss.sendBytes(buffer, n);
            n = ss.receiveBytes(buffer, sizeof(buffer));
        }
    }
    catch (Poco::Exception& exc)
    { std::cerr << "EchoConnection: " << exc.displayText() << std::endl; }
}
