#pragma once

#include "../state.h"

class Game;

namespace trait
{
	class GameState: public State
	{
	public:
		enum StateType : unsigned short int
		{
			ERROR = 0,
			MENU,
			PLAYING,
			PAUSED
		};

	public:
		GameState(StateType _stateType = ERROR);
		virtual ~GameState();

		static void SetOwner(Game* _owner) { pGame = _owner; }

		virtual void MainMenu() = 0;
		virtual void Pause() = 0;
		virtual void Play() = 0;

		virtual void Execute() = 0;
		virtual void DebugExecute() = 0;
		virtual void Print() = 0;
		virtual void DebugPrint() = 0;

	protected:
		static Game* pGame;
	};
}