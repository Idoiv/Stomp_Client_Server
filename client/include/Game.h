#pragma once

#include <string>
#include <map>
#include <utility>

using std::string;
using std::pair;

class Game{
private:
std::string gameName;
std::string gameUpdates;
std::map<string,string> teamA_Updates; 
std::map<string,string> teamB_Updates;
std::map<string,string> general_Stats; 
std::map< pair<int, string>,string> gameEventReport; // <<time,eventName>,description>



public:
Game(std::string gameName);
void addGeneralStats (pair<string,string> pair); 
void addTeamA_Stats (pair<string,string> pair); 
void addTeamB_Stats (pair<string,string> pair); 
void addGameEventReport (pair<int,string> pair, string description); 

std::string getGameName();
std::map<string,string> getGeneral_Stats();
std::map<string,string> getTeamA_Updates(); 
std::map<string,string> getTeamB_Updates();
std::map< pair<int, string>,string> getGameEventReport(); 










};