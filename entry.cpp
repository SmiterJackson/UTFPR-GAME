#include "game.h"

int main()
{
	setlocale(LC_ALL, "");

	Game* game = Game::GetInstance();
	if (game == nullptr)
		return -1;

	game->Execute();

	Game::DeconsInstance();
	return 1;
}