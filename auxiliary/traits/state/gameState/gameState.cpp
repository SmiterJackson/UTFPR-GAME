#include "gameState.h"
#include "../game.h"
using namespace manager;
using namespace trait;

Game* GameState::pGame = nullptr;

GameState::GameState(StateType _stateType) :
	State(_stateType)
{}
GameState::~GameState() 
{}