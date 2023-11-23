#pragma once

namespace auxiliary
{
	template<typename T>
	class AbstractIterator
	{
	public:
		AbstractIterator(unsigned int position = 0) :
			listPosition(position)
		{};
		virtual ~AbstractIterator()
		{};

		const unsigned int& ListPosition() const { return this->listPosition; };

		virtual void Restart() = 0;
		virtual void Next() = 0;
		virtual bool Ended() = 0;
		virtual T& Current() = 0;

	protected:
		unsigned int listPosition;
	};
}