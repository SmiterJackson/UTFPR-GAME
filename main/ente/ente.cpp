#include "ente.h"
#include "../game.h"

unsigned long long int Ente::counter = 0;
float& Ente::elapsedTime = Game::GetElapsedTime();

Ente::Ente(const unsigned short int _type, const unsigned short int _printPriority):
	id(counter++), 
	type(_type),
	printPriority(_printPriority)
{};
Ente::~Ente()
{};