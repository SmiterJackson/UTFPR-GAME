#include "gameState.h"
#include "../game.h"
using namespace manager;
using namespace trait;

EventManager::DebugFlagSubject* GameState::pDebugSubjectFlag = EventManager::DebugFlagSubject::GetInstance();
bool GameState::flagState = EventManager::DebugFlagSubject::GetInstance()->GetDebugFlag();
Game* GameState::pGame = nullptr;

GameState::GameState(StateType _stateType) :
	State(_stateType)
{
	pDebugSubjectFlag->AttachObs(this);
};
GameState::~GameState() 
{
	pDebugSubjectFlag->DettachObs(this);
};

void GameState::UpdateObs(const Subject* alteredSub)
{
	if (alteredSub == nullptr || alteredSub != pDebugSubjectFlag)
		return;

	flagState = pDebugSubjectFlag->GetDebugFlag();
};