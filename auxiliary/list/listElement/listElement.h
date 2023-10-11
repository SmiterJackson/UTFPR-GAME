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
		ListElement(const T* _info) :
			info(*_info), previous(nullptr), next(nullptr)
		{};
		ListElement(const T* _info, ListElement<T>* _previous) :
			info(*_info), previous(_previous), next(nullptr)
		{};
		ListElement(const T* _info, ListElement<T>* _previous, ListElement<T>* _next) :
			info(*_info), previous(_previous), next(_next)
		{};
		~ListElement()
		{};

		T& GetInfo() { return this->info; };
		ListElement<T>* const GetPrevious() { return this->previous; };
		ListElement<T>* const GetNext() { return this->next; };

		void SetInfo(const T* _info)
		{
			if (_info == nullptr)
				return;

			this->info = *_info;
		};
		void SetPrevious(ListElement<T>* _previous)
		{
			this->previous = _previous;
		};
		void SetNext(ListElement<T>* _next)
		{
			this->next = _next;
		};

		bool operator!= (ListElement<T>& other)
		{
			return this->GetInfo() != other.GetInfo();
		}
		bool operator== (ListElement<T>& other)
		{
			return this->GetInfo() == other.GetInfo();
		}

	private:
		T info;
		ListElement<T>* previous;
		ListElement<T>* next;
	};
}