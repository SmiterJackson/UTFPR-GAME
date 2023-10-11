#pragma once

#include "../iterator/iterator.h"
#include "listElement/listElement.h"

namespace auxiliary
{
	template<typename T>
	class MyList
	{
	public:
		MyList() :
			first(nullptr), last(nullptr), size(0ULL)
		{};
		MyList(const MyList<T>& _other) :
			first(nullptr), last(nullptr), size(0ULL)
		{
			unsigned int i = 0U;
			for (i = 0U; i < _other.size; i++)
				this->PushBack(_other[i]->GetInfo());
		};
		~MyList()
		{
			ListElement<T>* aux = nullptr;
			if (this->first != nullptr)
				this->first->SetPrevious(nullptr);

			while (this->last != nullptr)
			{
				aux = this->last->GetPrevious();
				delete this->last;
				this->last = aux;
			}
		};

		class Iterator : public AbstractIterator<T>
		{
		public:
			Iterator() :
				AbstractIterator<T>(),
				start(nullptr),
				pElement(nullptr)
			{};
				Iterator(ListElement<T>* _start) :
				AbstractIterator<T>(),
				start(_start),
				pElement(_start)
			{};
			virtual ~Iterator()
			{};

			unsigned int& ListPosition() { return this->listPosition; };

			void First()
			{
				this->pElement = this->start;
			};
			virtual void Next()
			{
				if (this->pElement != nullptr)
					if (this->pElement->GetNext() != nullptr)
						this->pElement = this->pElement->GetNext();
			};
			virtual void Previous()
			{
				if (this->pElement != nullptr)
					if (this->pElement->GetPrevious() != nullptr)
						this->pElement = this->pElement->GetPrevious();
			};
			virtual bool Ended()
			{
				return (this->pElement->GetNext() == this->start);
			};
			T& Current()
			{
				return this->pElement->GetInfo();
			};
			ListElement<T>* Element() 
			{
				return this->pElement; 
			}

			void operator= (const Iterator& _other)
			{
				this->start = _other.start;
				this->pElement = _other.pElement;
			};
			bool operator== (const Iterator& _other)
			{
				return (this->pElement == _other.pElement);
			};
			bool operator!= (const Iterator& _other)
			{
				return (this->pElement != _other.pElement);
			};

			T& operator* ()
			{
				return this->pElement->GetInfo();
			}
			T*& operator-> ()
			{
				return this->pElement->GetInfo();
			};

			Iterator& operator++ ()
			{
				this->Next();
				this->listPosition++;
				return (*this);
			}
			Iterator& operator++ (int val)
			{
				this->Next();
				this->listPosition++;
				return (*this);
			}
			Iterator& operator-- ()
			{
				this->Previous();
				this->listPosition--;
				return (*this);
			}
			Iterator& operator-- (int val)
			{
				this->Previous();
				this->listPosition--;
				return (*this);
			}

		protected:
			ListElement<T>* start;
			ListElement<T>* pElement;
		};
		class ReverseIterator : public Iterator
		{
		public:
			ReverseIterator() :
				Iterator()
			{};
			ReverseIterator(ListElement<T>* _start) :
				Iterator(_start)
			{};
			~ReverseIterator()
			{};

			void Next()
			{
				if (this->pElement != nullptr)
					if (this->pElement->GetPrevious() != nullptr)
						this->pElement = this->pElement->GetPrevious();
			};
			void Previous()
			{
				if (this->pElement != nullptr)
					if (this->pElement->GetNext() != nullptr)
						this->pElement = this->pElement->GetNext();
			};
			bool Ended()
			{
				return (this->pElement->GetPrevious() == this->start);
			};
		};

		void const PushBack(const T& item)
		{
			ListElement<T>* aux = new ListElement<T>(&item, this->last, this->first);

			if (aux == nullptr)
			{
				std::cerr << "N�o foi poss�vel alocar um novo elemento para a lista: MyList." << std::endl;
				return;
			}

			if (this->first == nullptr)
			{
				this->first = aux;
				this->last = aux;
			}
			else
			{
				this->last->SetNext(aux);
				this->last = aux;
				this->first->SetPrevious(this->last);
			}

			this->size++;
		};
		void PopBack()
		{
			ListElement<T>* aux = nullptr;

			if (this->last != nullptr)
			{
				aux = this->last->GetPrevious();

				if (aux != nullptr)
					aux->SetNext(nullptr);

				delete this->last;
				this->last = aux;

				this->size--;
			}
		};
		void PopAt(const unsigned int val)
		{
			unsigned int i = 0ULL;
			ListElement<T>* aux = this->first;

			if(val >= this->size)
			{
				std::cerr << "O valor dado excede tamanho da lista: MyList::PopAt(const unsigned long long int val)." << std::endl;
				return;
			}

			for (i = 0ULL; i < val; i++)
			{
				if (aux == nullptr)
				{
					std::cerr << "Não foi possível remover elemento da lista: MyList::PopAt(const unsigned long long int val)." << std::endl;
					return;
				}
				aux = aux->GetNext();
			}

			if (aux != nullptr)
			{
				if (aux->GetPrevious() != nullptr)
					aux->GetPrevious()->SetNext(aux->GetNext());

				if (aux->GetNext() != nullptr)
					aux->GetNext()->SetPrevious(aux->GetPrevious());

				if (aux == this->last && aux->GetPrevious() != nullptr)
					this->last = aux->GetPrevious();

				if (aux == this->first && aux->GetNext() != nullptr)
					this->first = aux->GetNext();
			}

			delete aux;
			this->size--;
		};
		void PopAt(Iterator& it)
		{
			unsigned int i = 0ULL;
			ListElement<T>* aux = it.Element();

			if (aux == nullptr)
			{
				std::cerr << "O iterador dado não contém dados -nullptr-: MyList::PopAt(Iterator)." << std::endl;
				return;
			}
			it++;

			if (aux->GetPrevious() != nullptr)
				aux->GetPrevious()->SetNext(aux->GetNext());

			if (aux->GetNext() != nullptr)
				aux->GetNext()->SetPrevious(aux->GetPrevious());

			if (aux == this->last && aux->GetPrevious() != nullptr)
				this->last = aux->GetPrevious();

			if (aux == this->first && aux->GetNext() != nullptr)
				this->first = aux->GetNext();

			delete aux;
			this->size--;
		};

		ListElement<T>* const GetFirst() { return this->first; };
		ListElement<T>* const GetLast() { return this->last; };

		Iterator begin()	{ return Iterator(this->first); };
		Iterator end()		{ return Iterator(this->last); };
		Iterator rbegin()	{ return ReverseIterator(this->last); };
		Iterator rend()		{ return ReverseIterator(this->first); };

		unsigned int Size() { return this->size; };

		ListElement<T>* operator[] (unsigned int val) const
		{
			unsigned int i = 0ULL;
			ListElement<T>* aux = this->first;

			if (aux == nullptr || val > this->size)
			{
				std::cerr << "A lista n�o cont�m elementos para iterar ou o valor " <<
					"requerido excede o n�mero de elementos, total(" << this->size << "): MyList::operator[]." << std::endl;
				return nullptr;
			}

			for (i = 0ULL; i < val; i++)
				aux = aux->GetNext();

			return aux;
		}
		MyList<T>& operator+= (const MyList<T>& _other)
		{
			Iterator it;

			for (it = _other.begin(); it != _other.end(); it++)
				this->PushBack(*it);

			return *this;
		}
		MyList<T>& operator+= (const T& element)
		{
			this->PushBack(element);
			return *this;
		}

	private:
		ListElement<T>* first;
		ListElement<T>* last;

		unsigned int size;
	};
}