#include "../include/ServerThread.h"

ServerThread::ServerThread(ConnectionHandler& ch, StompProtocol& p): connectionHandler(ch),protocol(p){


}



void ServerThread::readFromServer()
{   
    while (connectionHandler.isConnected()) {
     std::string answer="";
     std::vector<string> words;
    if(connectionHandler.getLine(answer)){
        if(answer.length()>0){
       connectionHandler.setConnection(protocol.handleAns(protocol.parseServer(answer),answer));
    }
    }
    else
        connectionHandler.setConnection(false);    
    
}

}
