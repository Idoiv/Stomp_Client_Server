#include "../include/Game.h"

Game::Game(std::string gamename) : gameName(gamename), gameUpdates(""), teamA_Updates(), teamB_Updates(), general_Stats(), gameEventReport()
{
}

std::string Game::getGameName()
{
  return gameName;
}

std::map<string, string> Game::getGeneral_Stats() { return general_Stats; }
std::map<string, string> Game::getTeamA_Updates() { return teamA_Updates; }
std::map<string, string> Game::getTeamB_Updates() { return teamB_Updates; }
std::map<pair<int, string>, string> Game::getGameEventReport() { return gameEventReport; }

void Game::addGeneralStats(pair<string, string> pair)
{
  general_Stats[pair.first] = pair.second;
}

void Game::addTeamA_Stats(pair<string, string> pair)
{
  teamA_Updates[pair.first] = pair.second;
}

void Game::addTeamB_Stats(pair<string, string> pair)
{
  teamB_Updates[pair.first] = pair.second;
}

void Game::addGameEventReport(pair<int, string> pair, string description)
{
  gameEventReport[pair] = description;
}
