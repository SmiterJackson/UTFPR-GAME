#pragma once

#include "../gameState.h"
#include "stage/stage.h"

namespace trait
{
	namespace state
	{
		class PlayState : public GameState
		{
		public:
			PlayState(stage::Stage* _stage = nullptr);
			~PlayState();

			void Execute();
			void DebugExecute();
			void Print();
			void DebugPrint();

			void MainMenu();
			void Pause();
			void Play();

		private:
			stage::Stage* stage;
		};
	}
}