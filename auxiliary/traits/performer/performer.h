#pragma once

#include "stdafx.h"

namespace trait
{
	/* 
	Classe em formato de template para criar derivada com uma função de proxy 
	para demais funções da derivada, por tempo delimitado atravez de 'actionLenght'
	*/
	template<class T>
	class ActionPerformer
	{
	public:
		ActionPerformer() :
			action(nullptr),
			actionTimer(0.f),
			actionLenght(0.f)
		{}
		virtual ~ActionPerformer()
		{}

		void PerformAction(const float& elapsedTime)
		{
			if (action == nullptr || this->actionLenght <= 0.f)
				return;

			T* t = static_cast<T*>(this);

			this->actionTimer += elapsedTime;
			if (this->actionTimer >= this->actionLenght)
			{
				this->actionLenght = 0.f;
				this->actionTimer = 0.f;

				this->action = nullptr;
			}
			else
				(t->*action)(); // Chama a função agregada à "action"
		}

	protected:
		void (T::*action)();

		float actionTimer;
		float actionLenght;
	};
}