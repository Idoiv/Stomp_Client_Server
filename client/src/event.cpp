#include "../include/event.h"
#include "../include/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
using std::string;
using json = nlohmann::json;

Event::Event(std::string team_a_name, std::string team_b_name, std::string name, int time,
             std::map<std::string, std::string> game_updates, std::map<std::string, std::string> team_a_updates,
             std::map<std::string, std::string> team_b_updates, std::string discription)
    : team_a_name(team_a_name), team_b_name(team_b_name), name(name),
      time(time), game_updates(game_updates), team_a_updates(team_a_updates),
      team_b_updates(team_b_updates), description(discription)
{
}

Event::~Event()
{
}

const std::string &Event::get_team_a_name() const
{
    return this->team_a_name;
}

const std::string &Event::get_team_b_name() const
{
    return this->team_b_name;
}

const std::string &Event::get_name() const
{
    return this->name;
}

int Event::get_time() const
{
    return this->time;
}

const std::map<std::string, std::string> &Event::get_game_updates() const
{
    return this->game_updates;
}

const std::map<std::string, std::string> &Event::get_team_a_updates() const
{
    return this->team_a_updates;
}

const std::map<std::string, std::string> &Event::get_team_b_updates() const
{
    return this->team_b_updates;
}

const std::string &Event::get_discription() const
{
    return this->description;
}

Event::Event(const std::string &frame_body) : team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description("")
{

    std::stringstream ss(frame_body);
    std::vector<std::string> lines;  
    std::string line;  
    int pos;  unsigned int i=1;

    while(std::getline(ss,line))
        lines.push_back(line);

   // team a name
    pos = lines[i].find(": ");
    lines[i].erase(0, pos+2);
    team_a_name =  lines[i];
    
    i++;

    // team b name
    pos = lines[i].find(": ");
    lines[i].erase(0, pos+2);
    team_b_name =  lines[i];

    i++;
    
    //event name
    pos = lines[i].find(": ");
    lines[i].erase(0, pos+2);
    name =  lines[i];

    i++;

    //time
    pos = lines[i].find(": ");
    lines[i].erase(0, pos+2);
    time = std::stoi(lines[i]);

    i++;
    i++;
   
    //updating general game updates

    while(lines[i] != "team a updates:")
    {    
        
        //erase spaces   
         while (lines[i][0] == ' ')
           { lines[i].erase(0, 1);}

        pos = lines[i].find(":");
        string first=lines[i].substr(0,pos);
        string second=lines[i].substr(pos+1);
        game_updates[first]= second;
        i++;
        
    }

    i++;

    //updating team a updates

    while(lines[i] != "team b updates:")
    {    
        
        //erase spaces
         while (lines[i][0] == ' ')
           { lines[i].erase(0, 1);}
      
        pos = lines[i].find(":");
        string first=lines[i].substr(0,pos);
        string second=lines[i].substr(pos+1);
        team_a_updates[first]= second;
        i++;
        
    }
    i++;
    //updating team b updates

    while(lines[i] != "description:")
    {    
        
        //erase spaces
        while (lines[i][0] == ' ')
           { lines[i].erase(0, 1);}
     
        pos = lines[i].find(":");
        string first=lines[i].substr(0,pos);
        string second=lines[i].substr(pos+1);
        team_b_updates[first]= second;
        i++;
        
    }

    i++;

     //updating discriptions
    while(i<lines.size() && lines[i]!="\0")
    {description+= lines[i] + "\n" ; i++;}




}

names_and_events parseEventsFile(std::string json_path)
{
    std::ifstream f(json_path);
    json data = json::parse(f);

    std::string team_a_name = data["team a"];
    std::string team_b_name = data["team b"];

    // run over all the events and convert them to Event objects
    std::vector<Event> events;
    for (auto &event : data["events"])
    {
        std::string name = event["event name"];
        int time = event["time"];
        std::string description = event["description"];
        std::map<std::string, std::string> game_updates;
        std::map<std::string, std::string> team_a_updates;
        std::map<std::string, std::string> team_b_updates;
        for (auto &update : event["general game updates"].items())
        {
            if (update.value().is_string())
                game_updates[update.key()] = update.value();
            else
                game_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team a updates"].items())
        {
            if (update.value().is_string())
                team_a_updates[update.key()] = update.value();
            else
                team_a_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team b updates"].items())
        {
            if (update.value().is_string())
                team_b_updates[update.key()] = update.value();
            else
                team_b_updates[update.key()] = update.value().dump();
        }
        
        events.push_back(Event(team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description));
    }

    names_and_events events_and_names{team_a_name, team_b_name, events};

    return events_and_names;
}