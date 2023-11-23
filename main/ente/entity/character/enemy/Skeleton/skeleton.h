#pragma once

#include "../enemy.h"
#include "../auxiliary/traits/performer/performer.h"

namespace character
{
	namespace enemy
	{
		class Skeleton : public Enemy, public trait::ActionPerformer<Skeleton>
		{
		private:
			enum Actions
			{
				IDLE = 0,
				WALK,
				ATTACK,
				DAMAGED,
				DIED
			};
			typedef std::vector<std::pair<int, Animation>> SkeletonAnimation;

		public:
			Skeleton();
			~Skeleton();

			const std::list<Hitbox> GetHitBoxes();

			void Execute();
			void DebugExecute();
			void DebugSelfPrint();

			void EntityColision(Entity* _other, const sf::Vector2f& intersection);
			void MapColision(const sf::Vector2f& intersection);

		protected:
			void Attack();
			void Dead();
			void Move();

		protected:
			static SkeletonAnimation classAnimations;
			sf::RectangleShape attackHitbox;
			DIRECTION direction;

			bool onGround;

			float searchTimer;
		};
	}
}