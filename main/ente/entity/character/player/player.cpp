#include "player.h"
#include "stage/stage.h"
#include "manager/graphicManager/graphicManager.h"
using namespace trait;
using namespace stage;
using namespace manager;
using namespace character;

#define OFFSET(l, t, r, b, x, y) Animation::OffSet(l, t, r, b, sf::Vector2f((x - (l + r)) / 2.f, (y - (t + b)) / 2.f))
									
#define P1TEXTURE "resources/textures/sprite_sheet/p1spritesheet.png"
#define P2TEXTURE "resources/textures/sprite_sheet/p2spritesheet.png"

#define TOKEN_SIZE sf::Vector2i(120, 80)
#define DEFAULT_TEXTURE sf::IntRect(sf::Vector2i(0, 1), TOKEN_SIZE)
#define HITBOX sf::Vector2f(25.f, 38.f)
#define SCALE 1.0f

#define INVENCIBILITY_FRAMES_TIME 1.f
#define P_TOTAL_LIFE 10U

#define JUMP_SPEED -5.0f
#define P_ACCELERATION 12.f
#define P_MAX_SPEED 3.f
#define P_MAX_SPEED_IN_AIR (P_MAX_SPEED * 0.75f)

EventManager::InputSubject*	Player::pInputSub = EventManager::InputSubject::GetInstance();
std::list<Player*>			Player::playerList = std::list<Player*>();

Player::PlayerAnimations	Player::animations = {
	std::pair<int, Animation>(Actions::IDLE,			Animation(0, 9,	1, TOKEN_SIZE, 0.2f,	true/*,	OFFSET(42, 40, 42, 0, TOKEN_SIZE.x, TOKEN_SIZE.y)*/)),
	std::pair<int, Animation>(Actions::IDLE_CROUCH,		Animation(8, 8,	3, TOKEN_SIZE, 1.f,		false)),
	std::pair<int, Animation>(Actions::WALK,			Animation(0, 9,	2, TOKEN_SIZE, 0.125f,	true/*,	OFFSET(42, 40, 42, 0, TOKEN_SIZE.x, TOKEN_SIZE.y)*/)),
	std::pair<int, Animation>(Actions::WALK_CROUCH,		Animation(0, 7,	3, TOKEN_SIZE, 0.15f,	true)),
	std::pair<int, Animation>(Actions::ATTACK,			Animation(0, 5,	4, TOKEN_SIZE, 0.5f,	false)),
	std::pair<int, Animation>(Actions::ATTACK_CROUCH,	Animation(6, 9,	4, TOKEN_SIZE, 0.5f,	false)),
	std::pair<int, Animation>(Actions::JUMP,			Animation(3, 5,	5, TOKEN_SIZE, 0.1f,	false)),
	std::pair<int, Animation>(Actions::FALL,			Animation(0, 2,	5, TOKEN_SIZE, 0.1f,	false)),
	std::pair<int, Animation>(Actions::DAMAGED,			Animation(9, 9,	3, TOKEN_SIZE, 0.5f,	false)),
	std::pair<int, Animation>(Actions::DIED,			Animation(0, 9,	0, TOKEN_SIZE, 0.5f,	false))
};

Player::Player() :
	Character(
		Type::PLAYER, HITBOX, sf::Vector2f(TOKEN_SIZE), Player::GetPlayerTexture(playerList.size()), 
		animations,	P_TOTAL_LIFE, INVENCIBILITY_FRAMES_TIME, 1.f, false, SCALE
	),
	keyToActions(Player::GetKeyMap(playerList.size())),
	shorter(false), 
	onGround(false), 
	crouching(false), 
	jump(false),
	walkLeft(false), 
	walkRight(false)
{
	pInputSub->AttachObs(this);
	playerList.emplace_back(this);
	this->body.setOrigin(this->body.getOrigin() + sf::Vector2f(0.f, 21.f));
};
Player::~Player()
{
	std::list<Player*>::iterator it;

	for (it = playerList.begin(); it != playerList.end(); it++)
	{
		if ((*it)->GetId() == this->id)
		{
			it = playerList.erase(it);
			break;
		}
	}
};

void Player::UpdateObs(const trait::Subject* alteredSub)
{
	keyToAction::iterator it;
	sf::Event _event;
	if (alteredSub == nullptr || alteredSub != this->pInputSub)
		return;

	_event = pInputSub->GetEvent();
	it = this->keyToActions.find(_event.key.code);
	if (it == this->keyToActions.end())
		return;

	if (_event.type == sf::Event::KeyPressed)
	{
		switch (it->second)
		{
		case ActionMap::WALK_RIGHT:
			this->walkRight = true;
			this->looking_right = true;
			break;
		case ActionMap::WALK_LEFT:
			this->walkLeft = true;
			this->looking_right = false;
			break;
		case ActionMap::JUMPED:
			this->jump = true;
			break;
		case ActionMap::CROUCH:
			this->crouching = true;
			break;
		default:
			break;
		}
	}
	else
	{
		switch (it->second)
		{
		case ActionMap::WALK_RIGHT:
			this->walkRight = false;
			break;
		case ActionMap::WALK_LEFT:
			this->walkLeft = false;
			break;
		case ActionMap::JUMPED:
			this->jump = false;
			break;
		case ActionMap::CROUCH:
			this->crouching = false;
			break;
		default:
			break;
		}
	}
};

void Player::Attack()
{

};
void Player::Move()
{
	float maxGravity = Stage::GetMaxGravityPull();
	sf::Vector2f accel = sf::Vector2f(
		P_ACCELERATION		* this->velocityCoeff * elapsedTime,
		Stage::GetGravity() * this->velocityCoeff * elapsedTime
	);

	if (this->crouching && this->onGround)
	{
		if (!this->shorter)
		{
			this->SetHitBoxSize(HITBOX - sf::Vector2f(0.f, HITBOX.y / 2.f));
			this->hitBox.setOrigin(this->hitBox.getSize() / 2.f);
			this->hitBox.setPosition(this->body.getPosition() + sf::Vector2f(0.f, HITBOX.y / 4.f));

			this->shorter = true;
		}

		accel.x *= 0.75f;
	}
	else if (this->shorter)
	{
		this->shorter = false;
		this->SetHitBoxSize(HITBOX);
		this->hitBox.setOrigin(this->hitBox.getSize() / 2.f);
		this->hitBox.setPosition(this->body.getPosition());
	}

	if (this->walkLeft && !this->walkRight || !this->walkLeft && this->walkRight)
	{
		/*
		if (this->walkRight && this->velocity < P_MAX_SPEED)
			this->velocity.x += accel.x;

		if (this->walkLeft && this->velocity > -P_MAX_SPEED)
			this->velocity.x -= accel.x;
		 
		if (this->crouching)
			this->next_ani = Actions::WALK_CROUCH;
		else
			this->next_ani = Actions::WALK;
		*/
		// versao branchless
		this->velocity.x += (
			( accel.x * (this->walkRight	&& velocity.x <  (P_MAX_SPEED ))) +
			(-accel.x * (this->walkLeft		&& velocity.x > -(P_MAX_SPEED )))
		);

		this->next_ani = (
			(Actions::WALK_CROUCH	* this->crouching) +
			(Actions::WALK			* !this->crouching)
		);
	}
	else
	{
		/*
		if (this->velocity.x < 0.f)
			this->velocity.x += accel.x;

		if (this->velocity.x > 0.f)
			this->velocity.x -= accel.x;

		if (this->velocity.x <= 0.2f && this->velocity.x >= -0.2f)
			this->velocity.x = 0.f;


		if (this->crouching)
			this->next_ani = Actions::IDLE_CROUCH;
		else
			this->next_ani = Actions::IDLE;
		*/
		// versao branchless
		this->velocity.x *= 0.90f;

		this->velocity.x = (
			(velocity.x * !(velocity.x <= 0.02f && velocity.x >= -0.02f))
		);

		this->next_ani = (
			(Actions::IDLE_CROUCH	* this->crouching) +
			(Actions::IDLE			* !this->crouching)
		);
	}

	if (this->jump && this->onGround && !this->crouching)
	{
		this->next_ani = Actions::JUMP;

		this->onGround = false;
		this->velocity.y = JUMP_SPEED;
	}

	if(!this->onGround)
	{
		/*
		if (this->velocity.x < 0.f)
			this->next_ani = Actions::JUMP;
		else if (this->velocity.x > 0.f)
			this->next_ani = Actions::FALL;
		*/
		//versao branchless
		this->velocity.x = (
			(velocity.x * (velocity.x <= P_MAX_SPEED_IN_AIR)) +
			(P_MAX_SPEED_IN_AIR * (velocity.x > P_MAX_SPEED_IN_AIR))
		);
		this->next_ani = (
			(Actions::JUMP * (this->velocity.y > 0.f)) +
			(Actions::FALL * (this->velocity.y < 0.f))
		);
	}

	this->velocity.y += accel.y;
	this->velocity.y = (
		(velocity.y * (velocity.y <= maxGravity)) +
		(maxGravity * (velocity.y >  maxGravity))
	);

	this->MovePosition(this->velocity.x, this->velocity.y);
};
void Player::Died()
{

};

void Player::EntityColision(Entity* _other, const sf::Vector2f& intersection)
{
	if (_other == nullptr)
		return;

	sf::Vector2f delta(intersection);

	if(_other->GetType() == Type::OBSTACLE)
	{
		if (intersection.x != 0.0) 
			this->velocity.x = 0.0;

		if (intersection.y != 0.0)
			this->velocity.y = 0.0;

		if (intersection.y <= 0.f)
			this->onGround = true;
	}
	else if(_other->GetType() == Type::ENEMY || _other->GetType() == Type::PROJECTILE)
	{
		
	}

	this->MovePosition(delta.x, delta.y);
};
void Player::MapColision(const sf::Vector2f& intersection)
{
	if (intersection.x != 0.0)
		this->velocity.x = 0.0;

	if (intersection.y != 0.0)
		this->velocity.y = 0.0;

	if (intersection.y <= 0.f)
		this->onGround = true;

	this->MovePosition(intersection.x, intersection.y);
};
void Player::CameraColision(const sf::Vector2f& intersection)
{
	this->velocity.x *= !(intersection.x != 0.f);

	this->MovePosition(intersection.x, 0.f);
};

Player::keyToAction Player::GetKeyMap(unsigned int size)
{
	typedef Player::keyToAction::value_type Value;

	const Player::keyToAction p1_ActionsMap = {
		Value(sf::Keyboard::D,		Player::ActionMap::WALK_RIGHT),
		Value(sf::Keyboard::A,		Player::ActionMap::WALK_LEFT),
		Value(sf::Keyboard::W,		Player::ActionMap::JUMPED),
		Value(sf::Keyboard::S,		Player::ActionMap::CROUCH)
	};
	const Player::keyToAction p2_ActionsMap = {
		Value(sf::Keyboard::Right,	Player::ActionMap::WALK_RIGHT),
		Value(sf::Keyboard::Left,	Player::ActionMap::WALK_LEFT),
		Value(sf::Keyboard::Up,		Player::ActionMap::JUMPED),
		Value(sf::Keyboard::Down,	Player::ActionMap::CROUCH)
	};

	if (size == 1)
		return p2_ActionsMap;

	return p1_ActionsMap;
};
const std::string Player::GetPlayerTexture(unsigned int size) 
{
	std::string str;

	switch (playerList.size())
	{
	case 1:
		str = P2TEXTURE;
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		str = P1TEXTURE;
		break;
	}

	return str;
};