#pragma once

#include "stdafx.h"

namespace trait
{
	class State
	{
	public:
		State(unsigned short int _stateValue = 0);
		virtual ~State();

		virtual void Execute() = 0;

		const unsigned short int GetState() { return this->state; }

	protected:
		unsigned short int state;
	};
}