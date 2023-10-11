#pragma once

namespace auxiliary
{
	template<typename T>
	class AbstractIterator
	{
	public:
		AbstractIterator() :
			listPosition(0)
		{};
		virtual ~AbstractIterator()
		{};

		unsigned int& ListPosition() { return this->listPosition; };

		virtual void First() = 0;
		virtual void Next() = 0;
		virtual void Previous() = 0;
		virtual bool Ended() = 0;
		virtual T& Current() = 0;

	protected:
		unsigned int listPosition;
	};
}