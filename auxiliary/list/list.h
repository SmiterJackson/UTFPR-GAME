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
			first(nullptr), last(nullptr), size(0U)
		{};
		MyList(const MyList<T>& _other) :
			first(nullptr), last(nullptr), size(0U)
		{
			unsigned int i = 0U, size = _other.size;
			for (i = 0U; i < size; i++)
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
			Iterator(ListElement<T>* _start = nullptr, const unsigned int position = 0, const bool _started = false) :
				AbstractIterator<T>(position),
				start(_start),
				pElement(_start),
				startPosition(position),
				started(_started)
			{};
			virtual ~Iterator()
			{};

			void Restart()
			{
				this->pElement = this->start;
				this->listPosition = this->startPosition;
				this->started = false;
			};
			virtual void Next()
			{
				if (this->pElement == nullptr)
				{
					std::cout << "Elemento nulo de lista, lista vazia." << std::endl;
					return;
				}
				else if (this->pElement->GetNext() == nullptr)
					return;

				this->pElement = this->pElement->GetNext();
				this->listPosition++;

				if (!this->started)
					this->started = true;
			};
			virtual bool Ended()
			{
				return (this->pElement == this->start);
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
				this->startPosition = _other.startPosition;
				this->started = _other.started;
			};
			bool operator== (const Iterator& _other)
			{
				return (this->pElement == _other.pElement) && (this->start && _other.started);
			};
			bool operator!= (const Iterator& _other)
			{
				return (this->pElement != _other.pElement) || (this->started != _other.started);
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
				return (*this);
			}
			Iterator& operator++ (int val)
			{
				this->Next();
				return (*this);
			}

		protected:
			ListElement<T>* start;
			ListElement<T>* pElement;

			unsigned int startPosition;
			bool started;
		};
		class ReverseIterator : public Iterator
		{
		public:
			ReverseIterator(const ListElement<T>& _start, unsigned int position = 0) :
				Iterator(_start, position)
			{};
			~ReverseIterator()
			{};

			void Next()
			{
				if (this->pElement == nullptr)
				{
					std::cout << "Elemento nulo de lista, lista vazia." << std::endl;
					return;
				}
				else if (this->pElement->GetPrevious() == nullptr)
					return;

				this->pElement = this->pElement->GetPrevious();
				this->listPosition--;
			};
			bool Ended()
			{
				auto aux = this->pElement->GetPrevious();
				return (aux == nullptr || aux == this->start);
			};
		};

		void const PushBack(T& item)
		{
			ListElement<T>* aux = new ListElement<T>(item, this->last, this->first);

			if (aux == nullptr)
			{
				std::cerr << "Não foi possível alocar um novo elemento para a lista: MyList." << std::endl;
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
					aux->SetNext(first);

				delete this->last;
				this->last = aux;
				this->size--;
			}
		};
		void PopAt(const unsigned int val)
		{
			unsigned int i = 0U;
			ListElement<T>* aux = this->first;

			if(val >= this->size || aux == nullptr)
			{
				std::cerr << "O valor dado excede tamanho da lista ou a lista é nula: MyList::PopAt(const unsigned int val)." << std::endl;
				return;
			}

			for (i = 0U; i < val; i++)
			{
				aux = aux->GetNext();

				if (aux == nullptr)
				{
					std::cerr << "Não foi possível remover elemento da lista: MyList::PopAt(const unsigned int val)." << std::endl;
					return;
				}
			}

			if (aux == this->last && aux->GetPrevious() != nullptr)
				this->last = aux->GetPrevious();

			if (aux == this->first && aux->GetNext() != nullptr)
				this->first = aux->GetNext();

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

			if (aux == this->last && aux->GetPrevious() != nullptr)
				this->last = aux->GetPrevious();

			if (aux == this->first && aux->GetNext() != nullptr)
				this->first = aux->GetNext();

			delete aux;
			this->size--;
		};

		ListElement<T>* const GetFirst() { return this->first; };
		ListElement<T>* const GetLast() { return this->last; };

		Iterator begin()	{ return Iterator(this->first, 0U); };
		Iterator end()		{ return Iterator(this->first, 0U, true); };
		Iterator rbegin()	{ return ReverseIterator(this->last, size); };
		Iterator rend()		{ return ReverseIterator(this->last, size, true); };

		unsigned int Size() { return this->size; };

		ListElement<T>* operator[] (const unsigned int val) const
		{
			unsigned int i = 0U;
			ListElement<T>* aux = this->first;

			if (aux == nullptr || val >= this->size)
			{
				std::cerr << "A lista é nula ou o valor requerido excede o número de elementos, total("
					<< this->size << "): MyList::operator[]." << std::endl;
				return nullptr;
			}

			for (i = 0U; i < val; i++)
			{
				if (aux == nullptr)
					break;

				aux = aux->GetNext();
			}

			return aux;
		}
		MyList<T>& operator+= (const MyList<T>& _other)
		{
			unsigned int i = 0U, size = _other.size;
			for (i = 0U; i < size; i++)
				this->PushBack(_other[i]->GetInfo());

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