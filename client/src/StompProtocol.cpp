#include "../include/StompProtocol.h"


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <tuple>

using std::string;
using std::vector;

StompProtocol::StompProtocol() :
    currSubId(0),
    receipt(0),
    topicToId(),
    receiptToCommand(),
    currUserName(),
    connectionHandler(),
    users(),
    connected(false) {
};

     StompProtocol::StompProtocol(const StompProtocol &other) :
    currSubId(other.currSubId),
    receipt(other.receipt),
    topicToId(other.topicToId),
    receiptToCommand(other.receiptToCommand),
    currUserName(other.currUserName),
    connectionHandler(other.connectionHandler),
    users(other.users),
    connected(other.connected)
{
    // perform a deep copy of the vector
    for (User* user : other.users) {
        users.push_back(new User(*user));
    }
}
   
 StompProtocol::StompProtocol(StompProtocol &&other) :
    currSubId(std::move(other.currSubId)),
    receipt(std::move(other.receipt)),
    topicToId(std::move(other.topicToId)),
    receiptToCommand(std::move(other.receiptToCommand)),
    currUserName(std::move(other.currUserName)),
    connectionHandler(other.connectionHandler),
    users(std::move(other.users)),
    connected(std::move(other.connected))
{
  
    other.users.clear();
}
     
       StompProtocol &StompProtocol::operator=(const StompProtocol &other) {
    if (this != &other) {
        for (User* user : other.users) {
            users.push_back(new User(*user));
        }
        currSubId = other.currSubId;
        receipt = other.receipt;
        topicToId = other.topicToId;
        receiptToCommand = other.receiptToCommand;
        currUserName = other.currUserName;
        connectionHandler = other.connectionHandler;
        connected = other.connected;
    }
    return *this;
}

StompProtocol &StompProtocol::operator=(StompProtocol &&other) {
    if (this != &other) {
        currSubId = std::move(other.currSubId);
        receipt = std::move(other.receipt);
        topicToId = std::move(other.topicToId);
        receiptToCommand = std::move(other.receiptToCommand);
        currUserName = std::move(other.currUserName);
        connectionHandler = std::move(other.connectionHandler);
        users = std::move(other.users);
        connected = std::move(other.connected);
    }
    return *this;
}

StompProtocol::~StompProtocol() {
    // deallocate the memory for each User object in the vector
    for (User* user : users) {
        delete user;
    }
}


void StompProtocol::handleRequest(string line)
{
    if(connectionHandler->isConnected()){
    vector<string> words;
    std::istringstream iss(line);
    std::string word;

    while (iss >> word)
    {
        words.push_back(word);
    }

    if (words[0] == "login" && connected)
            std::cout << "The client is already logged in, log out before trying again" << std::endl;
    else{
      if(connected){
    if (words[0] == "join" && words.size() == 2)
          join(words[1]);
    
    else if (words[0] == "exit" && words.size() == 2)
    {
        try // find subId if exists
        {
            int subscriptionId = topicToId.at(words[1]);
             exit(subscriptionId, words[1]);
        }
        catch (std::out_of_range &e)
        {   
             exit(-1, words[1]);
        }
    }
    else if (words[0] == "report" && words.size() == 2)
            report(words[1]); 
    else if (words[0] == "summary" && words.size() == 4)
            summary(words[1], words[2], words[3]);
    else if(words[0]=="logout")
         logout();
      }
    }
   		
    }

}

bool StompProtocol:: handleAns(vector<string> words, string msg){
     bool ans=true;     // if serverThread should be running;
       if (words[0] == "RECEIPT")
        ans =  handleReceipt(words[1]);
      else if(words[0]== "ERROR"){
              ans = false;
      }
       else if(words[0]=="MESSAGE")
          message(msg);

      return ans;

}


void StompProtocol::login(string user, string passcode)
{

    std::stringstream ss;
    ss << "CONNECT\n";
    ss << "accept-version:1.2"<<"\n";
    ss << "host:stomp.cs.bgu.ac.il"<<"\n";
    ss << "login:" << user << "\n";
    ss << "passcode:" << passcode << "\n\n";
    std::string output = ss.str();
    
     

    currUserName = user;
    
    if (!connectionHandler->sendFrameAscii(output,'\0')) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
           connectionHandler->setConnection(false);
        } //sending the event to the server
    
}

void StompProtocol::join(string destination)
{
    std::stringstream ss;
    ss << "SUBSCRIBE\n";
    ss << "destination:" << destination << "\n";
    ss << "id:" << currSubId << "\n";
    ss << "receipt:" << receipt << "\n";
    ss << "\n";
    std::string output = ss.str();
    receiptToCommand[receipt] = {0, destination, currSubId}; // mapping between receipt to command details. 0=join
    
    currSubId++;
    receipt++;
    if (!connectionHandler->sendFrameAscii(output,'\0')) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            connectionHandler->setConnection(false);
        } 
    
}

void StompProtocol::exit(int subId, string topic)
{

    std::stringstream ss;
    ss << "UNSUBSCRIBE\n";
    ss << "id:" << subId << "\n";
    ss << "receipt:" << receipt << "\n";
    ss << "\n";
    
    std::string output = ss.str();
    

    receiptToCommand[receipt] = {1, topic, subId}; // mapping between receipt to command details. 1=exit.

    receipt++;

    if (!connectionHandler->sendFrameAscii(output,'\0')) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            connectionHandler->setConnection(false);
        } 
        
}

void StompProtocol::report(string filePath)
{

    names_and_events nne = parseEventsFile(filePath);
    string destination = nne.team_a_name + "_" + nne.team_b_name;
    getOrCreateUser(currUserName);
    if(topicToId.count(destination)<=0)
        std::cout<<"cannot report"<<std::endl;

   
    

    for (unsigned int i = 0; i < nne.events.size(); i++)
    {
        std::stringstream ss;
        ss << "SEND"<< "\n";
        ss << "destination: " << destination << "\n";
        ss<<"\n";
        ss << "user: " + currUserName << "\n";
    
            ss << "team a: " << nne.events[i].get_team_a_name() << "\n";
            ss << "team b: " << nne.events[i].get_team_b_name() << "\n";
        
        ss << "event name: " << nne.events[i].get_name() << "\n";
        ss << "time: " << nne.events[i].get_time() << "\n";
        ss << "general game updates:"<< "\n";
        for (const std::pair<string, string> &pair : nne.events[i].get_game_updates())
        {
            ss << "    " << pair.first << ":" << pair.second << "\n";
        }

        ss << "team a updates:\n";
        for (const std::pair<string, string> &pair : nne.events[i].get_team_a_updates())
        {
            ss << "    " << pair.first << ":" << pair.second << "\n";
        }

        ss << "team b updates:\n";
        for (const std::pair<string, string> &pair : nne.events[i].get_team_b_updates())
        {
            ss << pair.first << ":" << pair.second << "\n";
        }

        ss << "description:\n";
        ss << nne.events[i].get_discription();

        ss << "\n";
        
        string output =  ss.str();
        

       if (!connectionHandler->sendFrameAscii(output,'\0')) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
           connectionHandler->setConnection(false);
        } 
        
    }
     
}

void StompProtocol::summary(string gameName, string user, string fileName)
{
    std::stringstream ss;
    //separte gameName to teams
    string teamA; string teamB;
    std::stringstream gn(gameName);
    std::getline(gn, teamA, '_');
    std::getline(gn, teamB, '_');

    Game *game= getOrCreateUser(user)->getGameOrCreate(gameName);
    
    
    
    ss << teamA << " vs " << teamB << "\n";
    ss << "Game stats:" << "\n";
    ss << "General stats:" << "\n";
    for (const std::pair<string, string> &pair : game->getGeneral_Stats())
        {
            ss << pair.first << ": " << pair.second << "\n";
        }

    ss << teamA << " stats:"  << "\n";
    for (const std::pair<string, string> &pair : game->getTeamA_Updates())
        {
            ss << pair.first << ": " << pair.second << "\n";
        }

    ss << teamB << " stats:"  << "\n";
     for (const std::pair<string, string> &pair : game->getTeamB_Updates())
        {
            ss << pair.first << ": " << pair.second << "\n";
        }

    ss << "Game event reports:" << "\n";
    for (const std::pair< std::pair<int, string>, string >  &pair : game->getGameEventReport())
        {
            ss << pair.first.first << " - " << pair.first.second<< ":" << "\n"<<"\n";
            ss << pair.second<<"\n"<<"\n";
        }

    string output =  ss.str();

    //write output to file
    std::ofstream file;
    file.open("./data/" + fileName, std::ios::out);
    if (file.is_open()) {
        file << output;
        file.close();
    }
}

bool StompProtocol::handleReceipt(string str)
{
    int receiptId;
    bool ans = true;

    auto pos = std::find(str.begin(), str.end(), ':');
    str.erase(str.begin(), pos + 1);
    receiptId = std::stoi(str);

    std::tuple<int, string, int> tuple = receiptToCommand[receiptId];
    int commandType = std::get<0>(tuple);
    string channel = std::get<1>(tuple);
    int mySubId = std::get<2>(tuple);

    
    if (commandType == 0)
    {
        std::cout<< " Joined channel: " + channel<< std::endl;
        topicToId[channel] = mySubId; // add subId and topic
      
    }

   if (commandType == 1)
    {
        std::cout<< " Exited channel: " + channel << std::endl; 
        topicToId.erase(channel); // erase subId and topic
    }

    if (commandType == 2) { // receipt logout command
         ans=false;
        topicToId.clear();
        receiptToCommand.clear();
        receipt=0;
        currSubId=0;
        currUserName="";
    } 
     if(ans)
       receiptToCommand.erase(receiptId); // erase receipt

    return ans;
}
void StompProtocol::logout()
{
    std::stringstream ss;

    ss << "DISCONNECT\n";
    ss << "receipt:" << receipt << "\n";
    ss << "\n";
    
    std::string output = ss.str();
    

    receiptToCommand[receipt] = {2, "", -1}; // mapping between receipt to command details. 1=exit.
    receipt++;
      connected=false;
   if (!connectionHandler->sendFrameAscii(output,'\0')) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            connectionHandler->setConnection(false);
        } 
}

User* StompProtocol::getOrCreateUser( string name)
{   
   
for(User *user: users)
  {   if(user->getName()==name)
         return user; 
        }

User *user= new User(name);
users.push_back(user);
return user;
  

}

void StompProtocol::message(string msg)
{   
    
    //erase the 5 first line of message to get only body
   
    for (int i = 0; i < 5; i++) {
    int pos = msg.find("\n");
    msg=msg.erase(0, pos+1);  
   
    
          }

    //getting userName from the body message
     int start = msg.find(":");
     int end =   msg.find("\n");
     string myUserName = msg.substr(start+2,end-start-2);
    
    // creating event out of msg body
    Event event = Event(msg);
    
    // creating the nne object
    vector<Event> v;
    v.push_back(event);
    names_and_events nne = {event.get_team_a_name(),event.get_team_b_name(), v};

    //adding information to the relevant game of the reporting user
    getOrCreateUser(myUserName)->addEvent(nne);

}

vector<string> StompProtocol::parseKeyboard(string input){

vector<string> words;
    std::istringstream iss(input);
    std::string word;

    while (iss >> word)
    {
        words.push_back(word);
    }
   return words;
}
void StompProtocol::setConnection(bool connection){
    connected=connection;
}
bool StompProtocol::isConnected(){
    return connected;
}

void StompProtocol:: setConHandler(ConnectionHandler* ch){
  connectionHandler=ch;
}


vector<string> StompProtocol::parseServer(string input){
    
    vector<string> words;
    std::istringstream iss(input);
    std::string word;

    while (iss >> word)
    {
        words.push_back(word);
    }
    return words;


}