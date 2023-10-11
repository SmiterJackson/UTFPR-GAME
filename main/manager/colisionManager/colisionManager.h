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
	private:
		struct HitBox
		{
			HitBox(const sf::Vector2f& _pos, const sf::Vector2f& _size):
				pos(_pos),
				size(_size)
			{}

			sf::Vector2f pos = sf::Vector2f(0.f, 0.f);
			sf::Vector2f size = sf::Vector2f(0.f, 0.f);
		};

	public:
		static ColisionManager* GetInstance();
		static void DeconstructInstance();

		static void SetOwner(stage::Stage* _owner) { pOwner = _owner; };

		void UpdateColisions(const float& elapsedTime);

		void Add(Entity* entity);
		void AddRange(EntityList* _entities);
		void Remove(const unsigned long long int entityId);
		void RemoveRange(const std::vector<unsigned long long int>& entitiesIds);

	private:
		ColisionManager();
		~ColisionManager();

		void CheckEntityColision(Entity* entity, Entity* other, float elapsed);
		void CheckMapColision(Entity* entity);
		void CheckCameraColision(character::Player* player);

		bool VectToHitBox(const sf::Vector2f& origin, const sf::Vector2f& dir, const HitBox& target,
						  sf::Vector2<short>& normal, float& delta);

	private:
		static stage::Stage* pOwner;
		static ColisionManager* instance;

		std::list<character::Player*>& players;
		std::list<obstacle::Obstacle*> obstacles;
		std::vector<character::enemy::Enemy*> enemies;

		float accumulator;
	};
}