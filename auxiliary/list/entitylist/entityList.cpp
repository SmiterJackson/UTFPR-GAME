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
void EntityList::AddRange(std::list<Entity*>& _entities)
{
	std::list<Entity*>::iterator it;

	for (it = _entities.begin(); it != _entities.end(); it++)
		this->entities.PushBack(*it);
};
void EntityList::RemoveEntity(const unsigned long long int entityId)
{
	MyList<Entity*>::Iterator it = this->entities.begin();
	
	while (it != this->entities.end())
	{
		if ((*it)->GetId() == entityId)
		{
			this->entities.PopAt(it);
			return;
		}
		it++;
	}

	if ((*it)->GetId() == entityId)
		this->entities.PopAt(it);
};
void EntityList::RemoveRange(const std::list<unsigned long long int>& entityId)
{
	std::list<unsigned long long int>::const_iterator cIt;
	MyList<Entity*>::Iterator it;

	for (cIt = entityId.cbegin(); cIt != entityId.cend(); cIt++)
	{
		for (it = this->entities.begin(); it != this->entities.end(); it++)
		{
			if (*cIt == (*it)->GetId())
			{
				this->entities.PopAt(it);
				break;
			}
		}
	}
};