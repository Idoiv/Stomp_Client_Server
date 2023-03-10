#include <stdlib.h>
#include <thread>
#include <vector>
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
#include "../include/ServerThread.h"


  
int main(int argc, char * argv[]) {
   std::vector <string> words;
   StompProtocol *protocol = new StompProtocol();
   std::cout<<"client started"<<std::endl;
    while (true) {
      if(!words.empty())
         words.clear();
       std::string word="";
      std::string firstInput = "";
      try {
        std::getline(std::cin, firstInput);
      } catch (const std::exception & e) {
        std::cout << "Error reading input: " << std::endl;
        continue;
      }
      
      std::istringstream iss(firstInput);
     
      while (iss >> word) {
        words.push_back(word);
      }
      
      if (words[0] == "login") {
        ConnectionHandler *connectionHandler = new ConnectionHandler("127.0.0.1", 7777);
        if (connectionHandler->connect()) {
      
          protocol->setConHandler(connectionHandler);
          protocol->login(words[2], words[3]);
          string line;
          connectionHandler->getFrameAscii(line, '\0');
          words = protocol->parseServer(line);
      
          if (words[0] == "CONNECTED") {
            std::cout << "Login successful\n" << std::endl;
            protocol->setConnection(true); // client is connected
             // create thread for server
            ServerThread* st = new ServerThread (*connectionHandler, *protocol);
            std::thread serverSocketThread( &ServerThread::readFromServer, st);
           // handling keyboard input
            while (connectionHandler->isConnected()) {
              std::string input;
                   if(protocol->isConnected()){
                try {
                  std::getline(std::cin, input);
                } catch (const std::exception & e) {
                  std::cout << "Error reading input: " << std::endl;
                  continue;
                }
             protocol->handleRequest(input);
                   }
            }
             serverSocketThread.join();
             connectionHandler->close();
             delete st;
             delete connectionHandler;
          } else {
            std::cerr << line << std::endl; ///print error msg from the server
            connectionHandler->close();
            delete connectionHandler;
          }
        } else {
          std::cerr << "Cannot connect to " << "127.0.0.1" << ":" << 7777 << std::endl;
          connectionHandler->close();
          delete connectionHandler;
        }
      }
        else{
            std::cerr<<"you must login first" << std::endl;
        }
    }
    delete protocol;
    return 0;
}

    