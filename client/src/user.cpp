#include "../include/user.h"


User::User(std::string username ):userName(username),games(){

}



// Copy constructor
User::User(const User& other) : userName(other.userName), games() {
    for (Game *game : other.games) {
        Game* newGame = new Game(*game);
        games.push_back(newGame);
    }
}

// Copy assignment operator
User& User::operator=(const User& other) {
    if (this != &other) {
        userName = other.userName;
        for (Game *game : games) {
            delete game;
        }
        games.clear();
        for (Game *game : other.games) {
            Game* newGame = new Game(*game);
            games.push_back(newGame);
        }
    }
    return *this;
}

// Move constructor
User::User(User&& other) : userName(std::move(other.userName)), games(std::move(other.games)) {}

// Move assignment operator
User& User::operator=(User&& other) {
    if (this != &other) {
        userName = std::move(other.userName);
        games = std::move(other.games);
    }
    return *this;
}

//destructor
User::~User() {
    for (Game* game : games) {
        delete game;
    }

}





void User::addGame(Game g){
    games.push_back(&g);
}
Game* User::getGameOrCreate(std::string gameName){
  for(Game *game: games){
    if(game->getGameName()==gameName)
        return game;
  }
      Game *game =   new Game(gameName);
      games.push_back(game);
      return game;

}

std::string User:: getName()
{
  return userName;
}

void User:: addEvent(names_and_events nne)
{
   string destination = nne.team_a_name + "_" + nne.team_b_name;
   Game *game = getGameOrCreate(destination);
 
  if(game!=nullptr)
  {
  for (unsigned int i = 0; i < nne.events.size(); i++)
    {
        for (const std::pair<string, string> &pair : nne.events[i].get_game_updates()) //adding game updates
        {
           game->addGeneralStats(pair);
        }


      for (const std::pair<string, string> &pair : nne.events[i].get_team_a_updates()) //adding team a stats
        {
           game->addTeamA_Stats(pair);
        }

          for (const std::pair<string, string> &pair : nne.events[i].get_team_b_updates()) //adding team b stats
        {
           game->addTeamB_Stats(pair);
        }

    std::pair<int,string> pair(nne.events[i].get_time(),nne.events[i].get_name());
    
     game->addGameEventReport(pair,nne.events[i].get_discription()); // adding description

    }  
  }
}

