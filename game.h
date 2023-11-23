#pragma once

#include "auxiliary/traits/state/gameState/MainMenu/mainMenuState.h"
#include "auxiliary/traits/state/gameState/Pause/pauseState.h"
#include "auxiliary/traits/state/gameState/Play/playState.h"
#include "manager/graphicManager/graphicManager.h"

class Game : public trait::Observer
{
public:
	static Game* GetInstance();
	static void DeconsInstance();

	static const unsigned short int GetState() { return instance->pState->GetState(); };
	static void SetState(trait::GameState* _newState)
	{ 
		if (instance->pState != nullptr)
			delete instance->pState;

		instance->pState = _newState;
		if (instance->pGraphicManager != nullptr)
			instance->pGraphicManager->SetGameState(instance->pState);
	};

	static float& GetElapsedTime() { return elapsedTime; };
	static trait::GameState* GetGameState() { return pState; };

	void UpdateObs(const trait::Subject* alteredSub);

	void MainMenu();
	void Pause();
	void Play();
	void Execute();

private:
	Game();
	~Game();

private:
	static float elapsedTime;

	manager::EventManager* pEventManager;
	manager::GraphicManager* pGraphicManager;
	manager::EventManager::DebugFlagSubject* pDebugSubjectFlag;

	static trait::GameState* pState;
	static Game* instance;

	bool flagState;
};