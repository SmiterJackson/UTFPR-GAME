#pragma once

#include "../gameState.h"

namespace trait
{
	namespace state
	{
		class MainMenuState : public GameState
		{
		public:
			MainMenuState();
			~MainMenuState();

			void Execute();
			void DebugExecute();
			void Print();
			void DebugPrint();

			void MainMenu();
			void Pause();
			void Play();
		};
	}
}