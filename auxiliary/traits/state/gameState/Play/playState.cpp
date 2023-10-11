#include "playState.h"
#include "../MainMenu/mainMenuState.h"
#include "../Pause/pauseState.h"
#include "stage/stage.h"
#include "../game.h"
using namespace trait::state;

PlayState::PlayState(stage::Stage* _stage):
	GameState(StateType::PLAYING),
	stage(_stage)
{
	
};
PlayState::~PlayState()
{};

void PlayState::Execute()
{
	if (this->stage == nullptr)
		return;

	if (!flagState)
		this->stage->Execute();
	else
		this->stage->DebugExecute();
};
void PlayState::DebugExecute()
{
	if (this->stage == nullptr)
		return;

};
void PlayState::Print()
{
	if (this->stage == nullptr)
		return;

	if (!flagState)
		this->stage->SelfPrint();
	else
		this->stage->DebugSelfPrint();
}

void PlayState::MainMenu()
{
	this->pGame->SetState(new MainMenuState());
};
void PlayState::Pause()
{
	this->pGame->SetState(new PauseState());
};
void PlayState::Play()
{};