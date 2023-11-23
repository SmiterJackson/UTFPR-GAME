#include "game.h"
#include "stage/initialStage/initialStage.h"
using namespace manager;
using namespace trait;

Game* Game::instance = nullptr;
float Game::elapsedTime   = 0.f;
GameState* Game::pState = nullptr;


Game* Game::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Game();
        if (instance == nullptr)
            std::cerr << "Não foi possível instanciar um Game. class Game." << std::endl;
    }

    return instance;
};
void Game::DeconsInstance()
{
    if (instance != nullptr)
        delete instance;
};

Game::Game():
    pEventManager(EventManager::GetInstance()),
    pGraphicManager(GraphicManager::GetInstance()),
    pDebugSubjectFlag(EventManager::DebugFlagSubject::GetInstance()),
    flagState(false)
{
    trait::GameState::SetOwner(this);

    this->pState = new state::PlayState(new stage::InitialStage());

    if(this->pGraphicManager != nullptr)
        this->pGraphicManager->SetGameState(this->pState);

    pDebugSubjectFlag->AttachObs(this);
};
Game::~Game()
{
    pDebugSubjectFlag->DettachObs(this);
};

void Game::UpdateObs(const Subject* alteredSub) 
{
    if (alteredSub == nullptr || alteredSub != pDebugSubjectFlag)
        return;

    flagState = pDebugSubjectFlag->GetDebugFlag();
}

void Game::MainMenu()
{
    this->pState->MainMenu();
};
void Game::Pause()
{
    this->pState->Pause();
};
void Game::Play()
{
    this->pState->Play();
};

void Game::Execute()
{
    sf::Clock clock;
    while (true)
    {
        this->elapsedTime = clock.restart().asSeconds();
        // normaliza o tempo elapsado para um dado limite (aproximadamente 30 fps)
        if (this->elapsedTime >= (1.f / 60.f))
            this->elapsedTime = (1.f / 60.f);

        this->pEventManager->Update();

        if (!flagState)
            this->pState->Execute();
        else
            this->pState->DebugExecute();

        this->pGraphicManager->Update();
    }
};