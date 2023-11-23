#include "pauseState.h"
#include "../MainMenu/mainMenuState.h"
#include "../Play/playState.h"
#include "../game.h"
using namespace trait::state;

PauseState::PauseState() :
	GameState(StateType::PAUSED)
{}
PauseState::~PauseState()
{}

void PauseState::Execute()
{

}
void PauseState::DebugExecute()
{

}
void PauseState::Print()
{

}
void PauseState::DebugPrint()
{

}

void PauseState::MainMenu()
{
	this->pGame->SetState(new MainMenuState());
}
void PauseState::Pause()
{}
void PauseState::Play()
{
	this->pGame->SetState(new PlayState());
}