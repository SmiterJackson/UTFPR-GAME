#pragma once

#include "manager/graphicManager/graphicManager.h"

const enum Type : const unsigned short int
{
	PLAYER = 0,
	ENEMY,
	PROJECTILE,
	OBSTACLE,
	STAGE,
	MOUSE,
	BACKGROUND,
	INTERFACE,
	BUTTON,
	UNDEFINED
};

// Classe mais básica das classes envolvidas no jogo
class Ente 
{
public:
	Ente(const Type _type = Type::UNDEFINED,
		 const PrintPriority _priority = PrintPriority::undefined);
	virtual ~Ente();

	const unsigned long long int GetId() const { return this->id; };
	const PrintPriority GetPriority() const { return this->priority; };
	const Type GetType() const { return this->type; };

	virtual void Execute() = 0;
	virtual void DebugExecute() = 0;
	virtual void SelfPrint() = 0;
	virtual void DebugSelfPrint() = 0;

	virtual bool operator> (const Ente& other)
	{
		return this->type > other.type;
	};
	virtual bool operator< (const Ente& other)
	{
		return this->type < other.type;
	};
	virtual bool operator!= (const Ente& other)
	{
		return this->id != other.id;
	};
	virtual bool operator== (const Ente& other)
	{
		return this->id == other.id;
	};

protected:
	static manager::GraphicManager* pGraphicManager;
	static float& elapsedTime;

	const unsigned long long int id;
	const PrintPriority priority;
	const Type type;

private:
	static unsigned long long int counter;
};