#include "ente.h"
#include "../game.h"
using namespace manager;

GraphicManager* Ente::pGraphicManager = GraphicManager::GetInstance();
float& Ente::elapsedTime = Game::GetElapsedTime();
unsigned long long int Ente::counter = 0;

Ente::Ente(const Type _type, const PrintPriority _priority):
	id(counter++), 
	type(_type),
	priority(_priority)
{};
Ente::~Ente()
{};