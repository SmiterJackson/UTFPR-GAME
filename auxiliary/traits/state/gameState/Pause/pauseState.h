#pragma once

#include "../gameState.h"

namespace trait
{
	namespace state
	{
		class PauseState : public GameState
		{
		public:
			PauseState();
			~PauseState();

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