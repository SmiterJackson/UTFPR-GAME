#pragma once

#include "../character.h"

namespace character
{
	class Player : public Character
	{
	protected:
		enum Actions
		{
			IDLE = 0,
			IDLE_CROUCH,
			WALK,
			WALK_CROUCH,
			ATTACK,
			ATTACK_CROUCH,
			JUMP,
			FALL,
			DAMAGED,
			DIED
		};
		enum ActionMap
		{
			WALK_LEFT = 0,
			WALK_RIGHT,
			JUMPED,
			CROUCH
		};
		typedef std::map<sf::Keyboard::Key, ActionMap> keyToAction;
		typedef std::vector<std::pair<int, Animation>> PlayerAnimations;

	public:
		Player();
		~Player();

		static std::list<Player*>& GetPlayerList() { return playerList; };
		static const unsigned int GetNumberOfPlayers() { return playerList.size(); };

		void UpdateObs(const trait::Subject* alteredSub);
		void EntityColision(Entity* _other, const sf::Vector2f& intersection);
		void MapColision(const sf::Vector2f& intersection);
		void CameraColision(const sf::Vector2f& intersection);

	protected:
		void Attack();
		void Move();
		void Died();

		static keyToAction GetKeyMap(unsigned int size);
		static const std::string GetPlayerTexture(unsigned int size);

	protected:
		static manager::EventManager::InputSubject* pInputSub;
		static PlayerAnimations animations;
		static std::list<Player*> playerList;

		keyToAction keyToActions;

		bool shorter;
		bool onGround;
		bool crouching;
		bool jump;
		bool walkLeft;
		bool walkRight;
	};
}