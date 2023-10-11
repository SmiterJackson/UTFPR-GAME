#include "enemyMelee.h"
#include "stage/stage.h"
#include "manager/graphicManager/graphicManager.h"
using namespace manager;
using namespace character;
using namespace character::enemy;

#define PATH "resources/textures/sprite_sheet/skeleton.png"
#define TOKEN_SIZE sf::Vector2i(64, 64)

#define SEARCH_TIME 0.4f
#define DIST_TO_ATTACK sf::Vector2f(51.f, 42.f)

#define HITBOX_SIZE sf::Vector2f(26.f, 33.f)
#define JUMP_SPEED -5.f
#define ACCELERATION 8.f

EnemyMelee::EnemyMeleeAnimation EnemyMelee::animations = {
	std::pair<int, Animation>(Actions::IDLE,	Animation(0, 3, 3, TOKEN_SIZE, 0.27f, true)),
	std::pair<int, Animation>(Actions::WALK,	Animation(0, 11, 2, TOKEN_SIZE, 0.165f, true)),
	std::pair<int, Animation>(Actions::ATTACK,	Animation(0, 12, 0, TOKEN_SIZE, 0.13f, true)),
	std::pair<int, Animation>(Actions::DAMAGED,	Animation(0, 2, 4, TOKEN_SIZE, 0.2f, true)),
	std::pair<int, Animation>(Actions::DIED,	Animation(0, 12, 1, TOKEN_SIZE, 0.2f, true))
};

EnemyMelee::EnemyMelee():
	Enemy(HITBOX_SIZE, sf::Vector2f(TOKEN_SIZE), PATH, animations, 10, 0.1f, 3.f, 200.f, false, 1.f),
	attackHitBox(),
	direction(A_STAR_RETURN_TYPE::ERROR),
	action(0),
	performingAction(false),
	onGround(false),
	searchTimer(0.f),
	elapsed(0.f),
	time(0.f)
{
	attackHitBox.setFillColor(sf::Color::Transparent);
	attackHitBox.setOutlineThickness(1.5f);
	attackHitBox.setSize(sf::Vector2f(0.f, 0.f));
	attackHitBox.setOrigin(sf::Vector2f(22.5f, 13.5f));
};
EnemyMelee::~EnemyMelee()
{};

void EnemyMelee::SelfPrint()
{
	this->UpdateAnimation(elapsedTime);
	GraphicManager::Draw(this->body);
};
void EnemyMelee::DebugSelfPrint()
{
	this->UpdateAnimation(elapsedTime);
	GraphicManager::Draw(this->body);
	GraphicManager::Draw(this->hitBox);
	GraphicManager::Draw(this->originCircle);
	GraphicManager::Draw(this->attackHitBox);
};

void EnemyMelee::Execute()
{
	this->elapsed += elapsedTime;
	this->Move();

	if (this->elapsed >= this->time)
	{
		if (time != 0)
		{
			this->elapsed -= this->time;
			this->time = 0.f;
		}
		else
			this->elapsed = 0.f;

		this->performingAction = false;

		this->Attack();
	}
};
void EnemyMelee::DebugExecute()
{
	sf::Vector2f newPos;

	this->elapsed += elapsedTime;
	this->Move();

	if (this->elapsed >= this->time)
	{
		if (time != 0)
		{
			this->elapsed -= this->time;
			this->time = 0.f;
		}
		else
			this->elapsed = 0.f;

		this->performingAction = false;

		this->Attack();
	}

	newPos = this->GetPosition();
	this->originCircle.setPosition(newPos.x, newPos.y);
};

void EnemyMelee::EntityColision(Entity* _other, const sf::Vector2f& intersection)
{
	if (_other == nullptr)
		return;

	if(_other->GetType() == Type::OBSTACLE && intersection.y < 0.f)
	{
		this->onGround = true;
		this->velocity.y = 0.f;
	}

	this->MovePosition(intersection.x, intersection.y);
};
void EnemyMelee::MapColision(const sf::Vector2f& intersection)
{
	if (intersection.x != 0.f)
		this->velocity.x = 0.f;

	if (intersection.y != 0.f)
		this->velocity.y = 0.f;

	if(intersection.y < 0.f)
		this->onGround = true;

	this->MovePosition(intersection.x, intersection.y);
};

void EnemyMelee::Attack()
{
	if (this->target == nullptr)
		return;

	sf::Vector2f pos(this->target->GetPosition() - this->GetPosition());

	if(std::abs(pos.x) <= DIST_TO_ATTACK.x && std::abs(pos.y) <= DIST_TO_ATTACK.y)
	{
		attackHitBox.setOutlineColor(this->pDebugFlagSub->GetDebugFlag() ? sf::Color::Red : sf::Color::Transparent);
		attackHitBox.setSize(sf::Vector2f(45.f, 27.f));

		this->time = this->animations[Actions::ATTACK].second.GetDuration();
		this->next_ani = Actions::ATTACK;
		this->performingAction = true;
		this->elapsed = 0.f;

		if (pos.x >= 0.f)
			this->looking_right = true;
		else
			this->looking_right = false;
	}
	else
		attackHitBox.setSize(sf::Vector2f(0.f, 0.f));
};
void EnemyMelee::Died()
{};
void EnemyMelee::Move()
{
	float gravity(stage::Stage::GetGravity());
	this->searchTimer += elapsedTime;

	if (this->target == nullptr)
		return;

	if (this->searchTimer >= SEARCH_TIME)
	{
		direction = AStarAlgorithm(
			stage::Stage::PositionToGrid(this->GetPosition()), 
			stage::Stage::PositionToGrid(this->target->GetPosition())
		);

		this->searchTimer -= SEARCH_TIME;
	}

	if(performingAction)
		direction = A_STAR_RETURN_TYPE::ERROR;
	else if (direction == A_STAR_RETURN_TYPE::UP && !onGround && !performingAction)
		direction = A_STAR_RETURN_TYPE::ERROR;
	else if (direction == A_STAR_RETURN_TYPE::UP_LEFT && !onGround && !performingAction)
		direction = A_STAR_RETURN_TYPE::LEFT;
	else if (direction == A_STAR_RETURN_TYPE::UP_RIGHT && !onGround && !performingAction)
		direction = A_STAR_RETURN_TYPE::RIGHT;

	switch (direction)
	{
	case A_STAR_RETURN_TYPE::LEFT:
		this->velocity += sf::Vector2f(-ACCELERATION * (1 - (2 * performingAction)), gravity) * elapsedTime;
		this->next_ani = Actions::WALK;
		this->looking_right = false;
		break;
	case A_STAR_RETURN_TYPE::RIGHT:
		this->velocity += sf::Vector2f(ACCELERATION * (1 - (2 * performingAction)), gravity) * elapsedTime;
		this->next_ani = Actions::WALK;
		this->looking_right = true;
		break;
	case A_STAR_RETURN_TYPE::DOWN_LEFT:
		this->velocity += sf::Vector2f(-ACCELERATION * (1 - (2 * performingAction)), gravity) * elapsedTime;
		this->next_ani = Actions::WALK;
		this->looking_right = false;
		break;
	case A_STAR_RETURN_TYPE::DOWN_RIGHT:
		this->velocity += sf::Vector2f(ACCELERATION * (1 - (2 * performingAction)), gravity) * elapsedTime;
		this->next_ani = Actions::WALK;
		this->looking_right = true;
		break;
	case A_STAR_RETURN_TYPE::DOWN:
		this->velocity += sf::Vector2f(0.f, gravity) * elapsedTime;
		this->next_ani = Actions::IDLE;
		break;
	case A_STAR_RETURN_TYPE::UP:
		this->onGround = false;
		this->velocity += sf::Vector2f(0.f, JUMP_SPEED);
		this->next_ani = Actions::IDLE;
		break;
	case A_STAR_RETURN_TYPE::UP_LEFT:
		this->onGround = false;
		this->velocity += sf::Vector2f(-ACCELERATION * elapsedTime, JUMP_SPEED);
		this->next_ani = Actions::WALK;
		this->looking_right = false;
		break;
	case A_STAR_RETURN_TYPE::UP_RIGHT:
		this->onGround = false;
		this->velocity += sf::Vector2f(ACCELERATION * elapsedTime, JUMP_SPEED);
		this->next_ani = Actions::WALK;
		this->looking_right = true;
		break;
	default:
		if(this->velocity.x > 0.05f )
			this->velocity += sf::Vector2f(-ACCELERATION, gravity) * elapsedTime;
		else if(this->velocity.x < -0.05f)
			this->velocity += sf::Vector2f(ACCELERATION, gravity) * elapsedTime;
		else
			this->velocity = sf::Vector2f(0.f, (gravity * elapsedTime) + this->velocity.y);

		if(!performingAction)
			this->next_ani = Actions::IDLE;
		break;
	}

	if (this->velocity.x > 1.6f)
		this->velocity.x = 1.6f;
	else if (this->velocity.x < -1.6f)
		this->velocity.x = -1.6f;

	if (this->velocity.y > stage::Stage::GetMaxGravityPull())
		this->velocity.y = stage::Stage::GetMaxGravityPull();

	this->MovePosition(this->velocity.x, this->velocity.y);
	this->attackHitBox.setPosition(
		this->GetPosition() + 
		sf::Vector2f(
			8.f * (1 - (2 * !this->looking_right)), -8.f
		)
	);
};