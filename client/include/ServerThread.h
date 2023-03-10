#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"


 
class ServerThread{
    public:
    ServerThread(ConnectionHandler& ch,StompProtocol& p);
    void readFromServer();
    private:
    ConnectionHandler& connectionHandler;
    StompProtocol& protocol;
    


};
