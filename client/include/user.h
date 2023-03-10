#pragma once

#include "../include/Game.h"
#include "../include/event.h"
#include <utility>
#include <string>
#include <vector>

class User{
private:
std::string userName;
std::vector<Game*> games;

public:
User(std::string username);

User(const User& other); //copy constructor
User& operator=(const User& other); //copy assignment operator
User(User&& other);//move constructor
User& operator=(User&& other);// Move assignment operator
~User(); // Destructor

void addGame(Game g);
Game* getGameOrCreate(std::string gameName);
std::string getName();
void addEvent(names_and_events nne);




};