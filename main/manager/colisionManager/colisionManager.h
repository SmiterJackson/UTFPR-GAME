#pragma once

#include "ente/entity/obstacle/obstacle.h"
#include "ente/entity/character/enemy/enemy.h"
#include "ente/entity/character/player/player.h"
#include "../auxiliary/list/entitylist/entityList.h"

namespace stage
{
	class Stage;
}

namespace manager
{
	class ColisionManager
	{
	public:
		static ColisionManager* GetInstance();
		static void DeconstructInstance();

		static void SetOwner(stage::Stage* _owner) { pOwner = _owner; };

		static bool RectsOverlaping(const sf::FloatRect& left, const sf::FloatRect& right);
		static bool RayCastTarget(Entity* entity, Entity* other, float elapsed, sf::Vector2f velocity = sf::Vector2f(0.0, 0.0));
		static bool ValidSpace(const sf::FloatRect& bounds);

		void UpdateColisions(const float& elapsedTime);

		void Add(Entity* entity);
		void AddRange(EntityList* _entities);
		void Remove(const unsigned long long int entityId);
		void RemoveRange(const std::list<unsigned long long int>& entitiesIds);

	private:
		ColisionManager();
		~ColisionManager();

		void CheckEntityColision(Entity* entity, Entity* other, float elapsed);
		void CheckCameraColision(character::Player* player);
		void CheckMapColision(Entity* entity);

		bool RayCast(const sf::Vector2f& origin, const sf::Vector2f& dir, Entity::Hitbox& trg);

	private:
		static stage::Stage* pOwner;
		static ColisionManager* instance;

		GraphicManager* pGraphicManager;

		std::list<character::Player*>& players;
		std::list<obstacle::Obstacle*> obstacles;
		std::vector<character::enemy::Enemy*> enemies;

		float accumulator;
	};
}