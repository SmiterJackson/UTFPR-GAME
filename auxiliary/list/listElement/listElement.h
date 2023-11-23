#pragma once

#include "stdafx.h"

namespace auxiliary
{
	template<typename T>
	class ListElement
	{
	public:
		ListElement() :
			info(), previous(nullptr), next(nullptr)
		{};
		ListElement(T& _info, ListElement<T>* _previous = nullptr, ListElement<T>* _next = nullptr) :
			info(_info), previous(_previous), next(_next)
		{};
		~ListElement()
		{
			if (previous != nullptr)
				this->previous->SetNext(this->next);

			if(next != nullptr)
				this->next->SetPrevious(this->previous);
		};

		T& GetInfo() { return this->info; };
		ListElement<T>* GetPrevious() { return this->previous; };
		ListElement<T>* GetNext() { return this->next; };

		void SetInfo(T& _info)
		{
			this->info = _info;
		};
		void SetPrevious(ListElement<T>* _previous)
		{
			this->previous = _previous;
		};
		void SetNext(ListElement<T>* _next)
		{
			this->next = _next;
		};

		bool operator!= (const ListElement<T>& other)
		{
			return info != info;
		}
		bool operator== (const ListElement<T>& other)
		{
			return info == info;
		}

	private:
		T info;
		ListElement<T>* previous;
		ListElement<T>* next;
	};
}