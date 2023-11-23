#pragma once

#include "../character.h"
#include "../auxiliary/traits/performer/performer.h"

namespace character
{
	class Player : public Character, public trait::ActionPerformer<Player>
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
			CROUCH,
			ATTACKED
		};
		typedef std::map<sf::Keyboard::Key, ActionMap> keyToAction;
		typedef std::vector<std::pair<int, Animation>> PlayerAnimations;

	public:
		Player();
		~Player();

		static std::list<Player*>& GetPlayerList() { return playerList; };
		static const unsigned int GetNumberOfPlayers() { return playerList.size(); };

		const std::list<Hitbox> GetHitBoxes();

		void Damage(unsigned short int damage = 1);

		void Execute();
		void DebugExecute();
		void SelfPrint();
		void DebugSelfPrint();

		void UpdateObs(const trait::Subject* alteredSub);
		void EntityColision(Entity* _other, const sf::Vector2f& intersection);
		void MapColision(const sf::Vector2f& intersection);
		void CameraColision(const sf::Vector2f& intersection);

	protected:
		void Attack();
		void Move();
		void FreeMove();
		void Crouch();
		void Dead();

		static keyToAction GetKeyMap(unsigned int size);
		static const std::string GetPlayerTexture(unsigned int size);

	protected:
		static manager::EventManager::InputSubject* pInputSub;
		static PlayerAnimations classAnimations;
		static std::list<Player*> playerList;

		keyToAction keyToActions;
		sf::RectangleShape attackHitbox;

		bool freeWalking;
		bool shorter;
		bool onGround;
		bool crouching;
		bool jump;
		bool walkLeft;
		bool walkRight;

		int points;
		float crouchTimer;
	};
}