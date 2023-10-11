#pragma once

#include "../player/player.h"
#include "../auxiliary/node/node.h"

namespace character
{
	namespace enemy
	{
		typedef std::list<character::Player*> PlayerList;
		
		class Enemy : public Character
		{
		protected:
			typedef std::vector<std::pair<sf::RectangleShape, sf::Text>> GridLine;
			typedef std::vector<GridLine> GridBlock;

			enum A_STAR_RETURN_TYPE : unsigned short int
			{
				ERROR = 0,
				UP,
				DOWN,
				LEFT,
				UP_LEFT,
				DOWN_LEFT,
				RIGHT,
				UP_RIGHT,
				DOWN_RIGHT
			};

		public:
			Enemy();
			Enemy(const sf::Vector2f _size, const sf::Vector2f _tokenSize, const std::string _texturePath,
				const AnimationSheet _animations = AnimationSheet(), const unsigned int _lifeAmount = 0U,
				const float _invcDuration = 0.f, const float _deathTime = 1.f, const float _agroRange = 1.f,
				const bool _isStatic = true, const float _scale = 1.0f);
			virtual ~Enemy();

			virtual void UpdateObs(const trait::Subject* alteredSub);
			virtual void UpdateTarget();

			virtual void EntityColision(Entity* _other, const sf::Vector2f& intersection) = 0;
			virtual void MapColision(const sf::Vector2f& intersection) = 0;

			virtual sf::RectangleShape GetAttackHitBox() = 0;

		protected:
			virtual void Attack() = 0;
			virtual void Died() = 0;
			static A_STAR_RETURN_TYPE AStarAlgorithm(const sf::Vector2i srcGridPos, const sf::Vector2i trgGridPos);
			static void tracePath(auxiliary::Node& trg);

		protected:
			static manager::EventManager::InputSubject* pInputSub;
			static std::vector<auxiliary::NodeVec>& grid;
			static const PlayerList& players;
			static GridBlock& gridBlock;

			static bool showAStar;

			bool agroing;
			float agroRange;
			Player* target;
		};
	}
}