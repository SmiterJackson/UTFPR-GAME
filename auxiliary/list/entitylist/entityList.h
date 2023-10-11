#pragma once

#include "ente/entity/entity.h"
#include "../list.h"

class EntityList
{
public:
	EntityList();
	~EntityList();

	unsigned int GetSize() { return this->entities.Size(); };

	void AddEntity(Entity* entity);
	void AddRange(std::list<Entity*>* _entities);
	void RemoveEntity(const unsigned long long int entityId);
	void RemoveRange(const std::vector<unsigned long long int>& entityId);

	Entity* operator[](const unsigned int& val)
	{
		return this->entities[val]->GetInfo();
	}
private:
	auxiliary::MyList<Entity*> entities;
};