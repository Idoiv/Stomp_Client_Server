#pragma once

#include "ConnectionHandler.h"
#include "user.h"
#include "Game.h"
#include "event.h"
#include <string>
#include <vector>
#include <fstream>
using std::string;
using std::pair;
using std::tuple;

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
int currSubId;
int receipt;
std::map<string,int> topicToId;
std::map< int , tuple<int,string,int> > receiptToCommand; // key: receiptId, value:command-type (0=join,1=exit,2=logout), channel, subId;
string currUserName;
ConnectionHandler* connectionHandler;
std:: vector<User*> users;
bool connected;

public:

    StompProtocol();
    StompProtocol(const StompProtocol &other);
    StompProtocol(StompProtocol &&other);
    StompProtocol& operator=(const StompProtocol &other);
    StompProtocol& operator=(StompProtocol &&other);
    ~StompProtocol();
    void handleRequest(string line);
    bool handleAns(std::vector<string> line, string msg);
	void login(string user, string password);
    void join(string destination);
    void exit(int subId, string topic);
    bool handleReceipt(string str);
    void report(string filePath);
    void summary(string gameName, string user, string fileName);
    void logout();
    User *getOrCreateUser(string name);
    std::vector<string>  parseKeyboard(string input);
    std::vector<string>  parseServer(string input);
    void message(string msg);
    void setConnection(bool connected);
    bool isConnected();
    void addUser(User x);
    void setConHandler(ConnectionHandler* connectionHandler);
};
