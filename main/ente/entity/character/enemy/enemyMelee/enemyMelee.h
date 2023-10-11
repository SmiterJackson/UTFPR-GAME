#pragma once

#include "../enemy.h"

namespace character
{
	namespace enemy
	{
		class EnemyMelee : public Enemy
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
			typedef std::vector<std::pair<int, Animation>> EnemyMeleeAnimation;

		public:
			EnemyMelee();
			~EnemyMelee();

			virtual void SelfPrint();
			virtual void DebugSelfPrint();
			virtual void Execute();
			virtual void DebugExecute();
			virtual void EntityColision(Entity* _other, const sf::Vector2f& intersection);
			virtual void MapColision(const sf::Vector2f& intersection);

			sf::RectangleShape GetAttackHitBox() { return this->attackHitBox; };

		protected:
			virtual void Attack();
			virtual void Died();
			virtual void Move();

		protected:
			static EnemyMeleeAnimation animations;
			sf::RectangleShape attackHitBox;
			A_STAR_RETURN_TYPE direction;

			unsigned int action;
			bool performingAction;
			bool onGround;

			float searchTimer;
			float elapsed;
			float time;
		};
	}
}