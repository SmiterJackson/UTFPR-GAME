#pragma once

#include "../enemy.h"
#include "../auxiliary/traits/performer/performer.h"

namespace character
{
	namespace enemy
	{
		class Demon : public Enemy, public trait::ActionPerformer<Demon>
		{
		private:
			enum Actions
			{
				IDLE_WALK = 0,
				ATTACK,
				DAMAGED,
				DIED
			};
			typedef std::vector<std::pair<int, Animation>> DemonAnimation;

		public:
			Demon();
			~Demon();

			void Execute();
			void DebugExecute();

			void EntityColision(Entity* _other, const sf::Vector2f& intersection);
			void MapColision(const sf::Vector2f& intersection);

		protected:
			void Attack();
			void Dead();
			void Move();

		protected:
			static DemonAnimation classAnimations;
			DIRECTION direction;

			float searchTimer;
			float dashTimer;
		};
	}
}