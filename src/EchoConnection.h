//
// Created by madrus on 26.06.16.
//

#ifndef PSERVER_ECHOCONNECTION_H
#define PSERVER_ECHOCONNECTION_H


#include <sys/socket.h>
#include <iostream>
#include <Poco/Net/TCPServerConnection.h>

class EchoConnection: public Poco::Net::TCPServerConnection {
public:
    EchoConnection(const Poco::Net::StreamSocket& s) : TCPServerConnection(s) { }

    void run();
};

#endif //PSERVER_ECHOCONNECTION_H
