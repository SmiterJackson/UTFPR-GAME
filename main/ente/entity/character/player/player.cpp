#include "player.h"
#include "stage/stage.h"
#include "manager/graphicManager/graphicManager.h"
using namespace trait;
using namespace stage;
using namespace manager;
using namespace character;
							
#define P1TEXTURE "resources/textures/sprite_sheet/p1spritesheet.png"
#define P2TEXTURE "resources/textures/sprite_sheet/p2spritesheet.png"

#define TOKEN_SIZE sf::Vector2i(120, 80)
#define DEFAULT_TEXTURE sf::IntRect(sf::Vector2i(0, 1), TOKEN_SIZE)
#define HITBOX sf::Vector2f(22.f, 38.f)
#define ATTACK_HITBOX sf::Vector2f(76.f, 39.f)
#define ATTACK_HITBOX_CROUCHED sf::Vector2f(68.f, 25.f)
#define SCALE 1.0f

#define ATTACK_OFFSET sf::Vector2f(16.f * (1 - (2 * !this->looking_right)), -2.f)
#define ATTACK_OFFSET_CROUCHED sf::Vector2f(10.f * (1 - (2 * !this->looking_right)), -2.f)

#define INVENCIBILITY_FRAMES_TIME 0.8f
#define CROUCH_TIMER 1 / 15.f
#define DESPAWN_TIME 5.f
#define P_TOTAL_LIFE 10i16

#define JUMP_SPEED -5.0f
#define P_ACCELERATION 12.f
#define P_MAX_SPEED 3.f
#define P_MAX_SPEED_IN_AIR (P_MAX_SPEED * 0.75f)

EventManager::InputSubject*	Player::pInputSub = EventManager::InputSubject::GetInstance();
std::list<Player*>			Player::playerList = std::list<Player*>();
Player::PlayerAnimations	Player::classAnimations = {
	std::pair<int, Animation>(Actions::IDLE,			Animation(0, 9,	1, TOKEN_SIZE, 0.16f,	true,	sf::Vector2f(5.f, 21.f))),
	std::pair<int, Animation>(Actions::IDLE_CROUCH,		Animation(8, 8,	3, TOKEN_SIZE, 0.8f,	false,	sf::Vector2f(5.f, 27.5f))),
	std::pair<int, Animation>(Actions::WALK,			Animation(0, 9,	2, TOKEN_SIZE, 0.1f,	true,	sf::Vector2f(5.f, 21.f))),
	std::pair<int, Animation>(Actions::WALK_CROUCH,		Animation(0, 7,	3, TOKEN_SIZE, 0.12f,	true,	sf::Vector2f(5.f, 27.5f))),
	std::pair<int, Animation>(Actions::ATTACK,			Animation(0, 5,	4, TOKEN_SIZE, 0.07f,	false,	sf::Vector2f(5.f, 21.f))),
	std::pair<int, Animation>(Actions::ATTACK_CROUCH,	Animation(6, 9,	4, TOKEN_SIZE, 0.07f,	false,	sf::Vector2f(5.f, 27.5f))),
	std::pair<int, Animation>(Actions::JUMP,			Animation(0, 2,	5, TOKEN_SIZE, 0.1f,	false,	sf::Vector2f(5.f, 21.f))),
	std::pair<int, Animation>(Actions::FALL,			Animation(3, 5,	5, TOKEN_SIZE, 0.1f,	false,	sf::Vector2f(5.f, 21.f))),
	std::pair<int, Animation>(Actions::DAMAGED,			Animation(9, 9,	3, TOKEN_SIZE, 0.02f,	true,	sf::Vector2f(5.f, 21.f))),
	std::pair<int, Animation>(Actions::DIED,			Animation(0, 9,	0, TOKEN_SIZE, 0.08f,	false,	sf::Vector2f(5.f, 21.f)))
};

Player::Player() :
	Character(
		Type::PLAYER, HITBOX, sf::Vector2f(TOKEN_SIZE), Player::GetPlayerTexture(playerList.size()), 
		classAnimations, P_TOTAL_LIFE, INVENCIBILITY_FRAMES_TIME, false, SCALE
	),
	ActionPerformer(),
	keyToActions(Player::GetKeyMap(playerList.size())),
	attackHitbox(),
	freeWalking(false),
	shorter(false), 
	onGround(false), 
	crouching(false), 
	jump(false),
	walkLeft(false), 
	walkRight(false),
	points(0),
	crouchTimer(0.f)
{
	pInputSub->AttachObs(this);
	playerList.emplace_back(this);

	attackHitbox.setFillColor(sf::Color::Transparent);
	attackHitbox.setOutlineThickness(-1.5f);
	attackHitbox.setSize(sf::Vector2f(0.f, 0.f));
	attackHitbox.setOrigin(ATTACK_HITBOX / 2.f);
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
	bool notFound = false;

	if (alteredSub == nullptr || alteredSub != this->pInputSub || this->next_ani == Actions::DIED)
		return;

	_event = pInputSub->GetEvent();
	it = this->keyToActions.find(_event.key.code);
	notFound = (it == this->keyToActions.end());

	bool ctrl_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
	bool shift_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);

	if (_event.type == sf::Event::KeyPressed)
	{
		if (_event.key.code == sf::Keyboard::F && ctrl_pressed && shift_pressed)
			freeWalking = !freeWalking;

		if(notFound)
			return;

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
		case ActionMap::ATTACKED:
			this->Attack();
			break;
		default:
			break;
		}
	}
	else
	{
		if (notFound)
			return;

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
const std::list<Player::Hitbox> Player::GetHitBoxes()
{
	sf::Vector2f size(this->GetSize());
	std::list<Hitbox> hitboxes;

	hitboxes.emplace_back(this->GetPosition(), size, HitboxType::BODY);

	size = this->attackHitbox.getSize();
	if(size.x != 0 && size.y != 0)
		hitboxes.emplace_back(this->attackHitbox.getPosition(), size, HitboxType::ATTACK_BOX);

	return hitboxes;
}

void Player::Damage(unsigned short int damage)
{
	if (this->invcTimer <= 0.f && this->life_counter > 0 && !freeWalking)
	{
		this->velocity.x = -(this->velocity.x / 2.5f);
		this->velocity.y = (JUMP_SPEED / 2.5f) * (1 - (2 * (this->velocity.y > 0.f)));

		this->life_counter -= damage;
		this->invcTimer = this->invcFrames;

		this->next_ani = Actions::DAMAGED;

		if (this->life_counter <= 0)
			this->Dead();
	}
}

void Player::Execute()
{
	this->Move();
	this->PerformAction(elapsedTime);

	if (invcTimer > 0.f)
	{
		invcTimer -= elapsedTime;
		this->invcTimer *= !(this->invcTimer <= 0.f);
	}
};
void Player::DebugExecute()
{
	sf::Vector2f newPos;
	std::stringstream ss;
	float round = 0.f;

	if (!freeWalking)
		this->Move();
	else
		this->FreeMove();
	this->PerformAction(elapsedTime);

	if (invcTimer > 0.f)
	{
		invcTimer -= elapsedTime;
		this->invcTimer *= !(this->invcTimer <= 0.f);

		round = int(invcTimer * 100 + 0.5f) / 100.f;

		ss << round << "s";
		invcText.setString(ss.str());
	}

	newPos = this->GetPosition();
	this->originCircle.setPosition(newPos.x, newPos.y);
	invcText.setPosition(newPos - (this->GetSize() / 2.f) - sf::Vector2f(-6.f, 16.f));
};
void Player::SelfPrint()
{
	this->UpdateAnimation(elapsedTime);
	sf::IntRect _body = body.getTextureRect();

	if (invcTimer > 0.f && int(invcTimer / flickTimer) % 2 == 0)
		body.setTextureRect(sf::IntRect(0, 0, 0, 0));

	pGraphicManager->Draw(this->body);
	body.setTextureRect(_body);
};
void Player::DebugSelfPrint()
{
	this->UpdateAnimation(elapsedTime);
	sf::IntRect _body = body.getTextureRect();

	if (invcTimer > 0.f && int(invcTimer / flickTimer) % 2 == 0)
		body.setTextureRect(sf::IntRect(0, 0, 0, 0));

	pGraphicManager->Draw(this->body);
	pGraphicManager->Draw(this->hitBox);
	pGraphicManager->Draw(this->originCircle);
	pGraphicManager->Draw(this->attackHitbox);
	pGraphicManager->Draw(this->invcText);
	body.setTextureRect(_body);
};

void Player::Attack()
{
	bool attacking = (this->next_ani == Actions::ATTACK || this->next_ani == Actions::ATTACK_CROUCH);
	int current = this->animations[this->next_ani].GetCurrent();
	sf::Vector2f size(this->crouching ? ATTACK_HITBOX_CROUCHED : ATTACK_HITBOX);

	if(this->action == nullptr && this->onGround)
	{
		this->next_ani = (
			(Actions::ATTACK_CROUCH * (this->crouching || this->shorter)) +
			(Actions::ATTACK * (!this->crouching && !this->shorter))
		);
		this->actionLenght = this->animations[this->next_ani].GetDuration();
		this->action = &Player::Attack;
	}
	else if (attacking && ((current >= 2 && current <= 4) || (current >= 7 && current <= 8)))
	{
		this->attackHitbox.setOutlineColor(
			this->pDebugFlagSub->GetDebugFlag() ?
			sf::Color::Red : sf::Color::Transparent
		);
		this->attackHitbox.setSize(size);
		this->attackHitbox.setOrigin(size / 2.f);
	}
	else
		this->attackHitbox.setSize(sf::Vector2f(0.f, 0.f));
};
void Player::Move()
{
	sf::Vector2f accel = sf::Vector2f(
		P_ACCELERATION	* this->velocityCoeff * elapsedTime,
		*gravity * this->velocityCoeff * elapsedTime
	);
	sf::Vector2f offset = this->crouching ? ATTACK_OFFSET_CROUCHED : ATTACK_OFFSET;

	if (this->next_ani == Actions::DIED)
		return;

	this->crouchTimer += elapsedTime;
	if (this->crouchTimer >= CROUCH_TIMER)
	{
		this->crouchTimer -= CROUCH_TIMER;
		this->Crouch();
	}

	if (this->shorter)
		accel.x *= 0.75f;

	if ((this->walkLeft && !this->walkRight || !this->walkLeft && this->walkRight) && this->action == nullptr)
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
			(Actions::WALK_CROUCH * (this->crouching || this->shorter)) +
			(Actions::WALK		  * (!this->crouching && !this->shorter))
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

		if(this->action == nullptr)
			this->next_ani = (
				(Actions::IDLE_CROUCH	* (this->crouching || this->shorter)) +
				(Actions::IDLE			* (!this->crouching && !this->shorter))
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
		(velocity.y  * (velocity.y <= *maxGravity)) +
		(*maxGravity * (velocity.y >  *maxGravity))
	);

	this->MovePosition(this->velocity);
	this->attackHitbox.setPosition(this->GetPosition() + offset);
};
void Player::FreeMove()
{
	sf::Vector2f accel = sf::Vector2f(
		P_ACCELERATION * this->velocityCoeff * elapsedTime,
		P_ACCELERATION * this->velocityCoeff * elapsedTime
	);
	sf::Vector2f offset = this->crouching ? ATTACK_OFFSET_CROUCHED : ATTACK_OFFSET;

	if ((this->walkLeft && !this->walkRight || !this->walkLeft && this->walkRight) && this->action == nullptr)
	{
		this->velocity.x += (
			(accel.x * (this->walkRight && velocity.x < (P_MAX_SPEED))) +
			(-accel.x * (this->walkLeft && velocity.x > -(P_MAX_SPEED)))
			);

		this->next_ani = (
			(Actions::WALK_CROUCH * (this->crouching || this->shorter)) +
			(Actions::WALK * (!this->crouching && !this->shorter))
			);
	}
	else
	{
		this->velocity.x *= 0.90f;

		this->velocity.x = (
			(velocity.x * !(velocity.x <= 0.02f && velocity.x >= -0.02f))
			);

		if (this->action == nullptr)
			this->next_ani = (
				(Actions::IDLE_CROUCH * (this->crouching || this->shorter)) +
				(Actions::IDLE * (!this->crouching && !this->shorter))
				);
	}

	if ((this->jump && !this->crouching || !this->jump && this->crouching) && this->action == nullptr)
	{
		this->velocity.y += (
			(accel.y * (this->crouching && velocity.y < (P_MAX_SPEED))) +
			(-accel.y * (this->jump && velocity.y > -(P_MAX_SPEED)))
			);

		this->next_ani = Actions::IDLE;
	}
	else
	{
		this->velocity.y *= 0.90f;

		this->velocity.y = (
			(velocity.y * !(velocity.y <= 0.02f && velocity.y >= -0.02f))
			);

		if (this->action == nullptr)
			this->next_ani = Actions::IDLE;
	}

	this->MovePosition(this->velocity);
	this->attackHitbox.setPosition(this->GetPosition() + offset);
}
void Player::Crouch()
{
	sf::FloatRect newHitBox;
	sf::Vector2f size;
	sf::Vector2f pos;

	if (this->crouching && this->onGround && !this->shorter)
	{
		size = HITBOX * sf::Vector2f(0, 0.5f);

		this->SetHitBoxSize(HITBOX - size + sf::Vector2f(2.f, 6.f));
		this->hitBox.setOrigin(this->hitBox.getSize() / 2.f);
		this->hitBox.setPosition(this->body.getPosition() + sf::Vector2f(0.f, HITBOX.y / 4.f));

		this->shorter = true;
	}
	else if (this->shorter && !this->crouching)
	{
		pos = this->GetPosition();
		size = HITBOX / 2.f;
		newHitBox = sf::FloatRect(
			pos.x - size.x,
			pos.y - (size.y * 1.5f),
			pos.x + size.x,
			pos.y + size.y / 2.f
		);

		if (ColisionManager::ValidSpace(newHitBox))
		{
			this->SetHitBoxSize(HITBOX);
			this->hitBox.setOrigin(this->hitBox.getSize() / 2.f);
			this->hitBox.setPosition(this->body.getPosition());

			this->shorter = false;
		}
	}
};
void Player::Dead()
{
	int current = this->animations[this->next_ani].GetCurrent();

	if (this->action == nullptr && this->onGround)
	{
		this->next_ani = Actions::DIED;
		this->actionLenght = INFINITY;
		this->action = &Player::Dead;
	}
	else if (current >= 9 && this->actionTimer >= DESPAWN_TIME)
	{
		if(playerList.size() >= 2)
			pStage->Despawn(this->id);
		//else
		// colocar para entrar em uma tela de pós-morte
	}
};

void Player::EntityColision(Entity* _other, const sf::Vector2f& intersection)
{
	if (_other == nullptr || this->freeWalking)
		return;

	sf::Vector2f delta(intersection);
	sf::FloatRect rect;
	sf::Vector2f pos, size;

	if(_other->GetType() == Type::OBSTACLE)
	{
		if (intersection.x != 0.0)
			this->velocity.x = 0.0;

		if (intersection.y != 0.0)
			this->velocity.y = *gravity * this->velocityCoeff * elapsedTime;

		if (intersection.y < 0.f)
			this->onGround = true;
	}
	else if (_other->GetType() == Type::ENEMY || _other->GetType() == Type::PROJECTILE)
	{
		delta = sf::Vector2f(0.f, 0.f);

		size = this->attackHitbox.getSize();
		pos = this->attackHitbox.getPosition();
		rect = sf::FloatRect(
			pos.x - size.x,
			pos.y - size.y,
			pos.x + size.x,
			pos.y + size.y
		);

		if (size.x != 0 && size.y != 0 && ColisionManager::RectsOverlaping(rect, _other->GetBounds()))
			static_cast<Character*>(_other)->Damage(1);
	}
	else if (this->invcTimer > 0.f)
		delta = sf::Vector2f(0.f, 0.f);

	this->MovePosition(delta);
};
void Player::MapColision(const sf::Vector2f& intersection)
{
	if (intersection.x != 0.0)
		this->velocity.x = 0.0;

	if (intersection.y != 0.0)
		this->velocity.y = *gravity * this->velocityCoeff * elapsedTime;

	if (intersection.y < 0.f)
		this->onGround = true;

	this->MovePosition(intersection);
};
void Player::CameraColision(const sf::Vector2f& intersection)
{
	sf::Vector2f delta(intersection.x, 0.f);

	this->velocity.x *= !(intersection.x != 0.f);

	this->MovePosition(delta);
};

Player::keyToAction Player::GetKeyMap(unsigned int size)
{
	typedef Player::keyToAction::value_type Value;

	const Player::keyToAction p1_ActionsMap = {
		Value(sf::Keyboard::D,		Player::ActionMap::WALK_RIGHT),
		Value(sf::Keyboard::A,		Player::ActionMap::WALK_LEFT),
		Value(sf::Keyboard::W,		Player::ActionMap::JUMPED),
		Value(sf::Keyboard::S,		Player::ActionMap::CROUCH),
		Value(sf::Keyboard::C,		Player::ActionMap::ATTACKED)
	};
	const Player::keyToAction p2_ActionsMap = {
		Value(sf::Keyboard::Right,	Player::ActionMap::WALK_RIGHT),
		Value(sf::Keyboard::Left,	Player::ActionMap::WALK_LEFT),
		Value(sf::Keyboard::Up,		Player::ActionMap::JUMPED),
		Value(sf::Keyboard::Down,	Player::ActionMap::CROUCH),
		Value(sf::Keyboard::Numpad1,	Player::ActionMap::ATTACKED)
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
	default:
		str = P1TEXTURE;
		break;
	}

	return str;
};