#include "subject.h"
using namespace trait;

Subject::Subject():
	observers()
{};
Subject::~Subject()
{};

void Subject::AttachObs(Observer* obs)
{
	this->observers.emplace_back(obs);
};
void Subject::DettachObs(Observer* obs)
{
	std::list<Observer*>::iterator it;

	for (it = this->observers.begin(); it != this->observers.end(); it++)
		if (*it == obs)
		{
			it = this->observers.erase(it);
			break;
		}
};
void Subject::NotifyAllObs()
{
	std::list<Observer*>::iterator it;

	for (it = this->observers.begin(); it != this->observers.end(); it++)
		(*it)->UpdateObs(this);
};