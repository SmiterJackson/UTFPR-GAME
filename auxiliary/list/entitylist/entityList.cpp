#include "entityList.h"
using namespace auxiliary;

EntityList::EntityList()
{};
EntityList::~EntityList()
{};

void EntityList::AddEntity(Entity* entity)
{
	if (entity == nullptr)
		return;

	this->entities.PushBack(entity);
};
void EntityList::AddRange(std::list<Entity*>* _entities)
{
	std::list<Entity*>::iterator it;

	if (_entities == nullptr)
		return;

	for (it = _entities->begin(); it != _entities->end(); it++)
		this->entities.PushBack(*it);
};
void EntityList::RemoveEntity(const unsigned long long int entityId)
{
	MyList<Entity*>::Iterator it = this->entities.begin()--;

	do
	{
		it++;
		if ((*it)->GetId() == entityId)
		{
			this->entities.PopAt(it);
			break;
		}
	} while (it != this->entities.end());
};
void EntityList::RemoveRange(const std::vector<unsigned long long int>& entityId)
{
	std::vector<unsigned long long int>::const_iterator cIt;
	MyList<Entity*>::Iterator it = this->entities.begin()--;

	for (cIt = entityId.cbegin(); cIt != entityId.cend(); cIt++)
	{
		do
		{
			it++;
			if ((*it)->GetId() == (*cIt))
			{
				this->entities.PopAt(it);
			}
		} while (it != this->entities.end());
	}
};