#pragma once

#include "../observer/observer.h"

namespace trait
{
	class Subject 
	{
	public:
		Subject();
		virtual ~Subject();

		virtual void AttachObs(Observer* obs);
		virtual void DettachObs(Observer* obs);
		virtual void NotifyAllObs();

	protected:
		std::list<Observer*> observers;
	};
}