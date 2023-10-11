#pragma once

#include "stdafx.h"

const enum PrintPriority : unsigned short int
{
	undefined = 0,
	background,
	obstacles,
	characters,
	projectiles,
	interfaces,
	buttons
};
const enum Type : unsigned short int
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

class Ente 
{
public:
	Ente(const unsigned short int _type = Type::UNDEFINED,
		const unsigned short int _printPriority = PrintPriority::undefined);
	virtual ~Ente();

	virtual const unsigned long long int GetId() const { return this->id; };
	const unsigned short int GetType() const { return this->type; };
	const unsigned short int GetPrintPriority() const { return this->printPriority; };

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

protected:
	static float& elapsedTime;

	const unsigned long long int id;
	const unsigned short int type;
	const unsigned short int printPriority;

private:
	static unsigned long long int counter;
};