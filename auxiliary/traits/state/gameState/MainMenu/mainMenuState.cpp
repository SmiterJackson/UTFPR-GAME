#include "mainMenuState.h"
#include "../Pause/pauseState.h"
#include "../Play/playState.h"
#include "../game.h"
using namespace trait::state;

// definir método para escolher fase e iniciar corretamente o 'PLAYSTATE'

MainMenuState::MainMenuState() :
	GameState(StateType::MENU)
{}
MainMenuState::~MainMenuState()
{}

void MainMenuState::Execute()
{

}
void MainMenuState::DebugExecute()
{

}
void MainMenuState::Print()
{

}
void MainMenuState::DebugPrint()
{

}

void MainMenuState::MainMenu()
{}
void MainMenuState::Pause()
{
	this->pGame->SetState(new PauseState());
}
void MainMenuState::Play()
{
	this->pGame->SetState(new PlayState());
}