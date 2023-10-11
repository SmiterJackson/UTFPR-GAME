#pragma once

#include "stdafx.h"

namespace trait
{
	class Subject;

	class Observer
	{
	public:
		Observer();
		virtual ~Observer();

		virtual void UpdateObs(const Subject* alteredSub) = 0;
	};
}