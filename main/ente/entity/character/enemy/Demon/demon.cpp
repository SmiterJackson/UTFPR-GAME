#include "demon.h"
#include "stage/stage.h"
#include "manager/graphicManager/graphicManager.h"
using namespace manager;
using namespace character;
using namespace character::enemy;

#define PATH "resources/textures/sprite_sheet/demon.png"
#define TOKEN_SIZE sf::Vector2i(64, 64)

#define SEARCH_TIME 0.05f
#define DIST_TO_ATTACK sf::Vector2f(128.f, 128.f)

#define HITBOX_SIZE sf::Vector2f(42.f, 31.f)
#define ACCELERATION 8.f
#define DAMAGE 1

#define MAX_VELOCITY (ACCELERATION * 0.2f)
#define ATTACK_VELOCITY (2.f * MAX_VELOCITY)

#define DASH_TIMER 3.f

Demon::DemonAnimation Demon::classAnimations = {
	std::pair<int, Animation>(Actions::IDLE_WALK,	Animation(0, 5, 0,	TOKEN_SIZE, 0.27f, true, sf::Vector2f(0.f, 4.f))),
	std::pair<int, Animation>(Actions::ATTACK,		Animation(0, 5, 1,	TOKEN_SIZE, 0.2f, true)),
	std::pair<int, Animation>(Actions::DAMAGED,		Animation(0, 3, 2,	TOKEN_SIZE, 0.2f, true)),
	std::pair<int, Animation>(Actions::DIED,		Animation(0, 7, 3,	TOKEN_SIZE, 0.2f, true))
};

Demon::Demon() :
	Enemy(HITBOX_SIZE, sf::Vector2f(TOKEN_SIZE), PATH, classAnimations, 3, 0.f, 200.f, false, 1.f),
	ActionPerformer(),
	direction(DIRECTION::ERROR),
	searchTimer(0.f),
	dashTimer(0.f)
{}
Demon::~Demon()
{}

void Demon::Execute()
{
	sf::Vector2f dist;

	this->Move();
	if (this->target != nullptr && this->dashTimer <= 0.0)
	{
		dist = sf::abs(this->target->GetPosition() - this->GetPosition());

		if (dist <= DIST_TO_ATTACK)
			this->Attack();
	}
	else
		this->dashTimer -= elapsedTime;

	if (invcTimer > 0.f)
	{
		invcTimer -= elapsedTime;
		this->invcTimer *= !(this->invcTimer <= 0.f);
	}

	this->PerformAction(elapsedTime);
}
void Demon::DebugExecute()
{
	sf::Vector2f newPos, dist;
	std::stringstream ss;
	float round = 0.f;

	this->Move();
	if (this->target != nullptr && this->dashTimer <= 0.0)
	{
		dist = sf::abs(this->target->GetPosition() - this->GetPosition());

		if (dist <= DIST_TO_ATTACK)
			this->Attack();
	}
	else
		this->dashTimer -= elapsedTime;

	if (invcTimer > 0.f)
	{
		invcTimer -= elapsedTime;
		this->invcTimer *= !(this->invcTimer <= 0.f);

		round = int(invcTimer * 100 + 0.5f) / 100.f;

		ss << round << "s";
		invcText.setString(ss.str());
	}

	this->PerformAction(elapsedTime);

	newPos = this->GetPosition();
	this->originCircle.setPosition(newPos.x, newPos.y);
	this->invcText.setPosition(newPos - (this->GetSize() / 2.f) - sf::Vector2f(-6.f, 16.f));
}

void Demon::EntityColision(Entity* _other, const sf::Vector2f& intersection)
{
	if (_other == nullptr)
		return;

	sf::Vector2f delta(intersection);

	if (_other->GetType() == Type::OBSTACLE)
	{
		if (intersection.x != 0.f)
			this->velocity.x = 0.f;

		if (intersection.y != 0.f)
			this->velocity.y = 0.f;
	}
	else if (_other->GetType() == Type::PLAYER)
	{
		delta *= 0.f;
		this->velocity.x = 0.f;

		if (ColisionManager::RectsOverlaping(this->GetBounds(), _other->GetBounds()))
			static_cast<Character*>(_other)->Damage(DAMAGE);
	}

	this->MovePosition(delta);
}
void Demon::MapColision(const sf::Vector2f& intersection)
{
	if (intersection.x != 0.f)
		this->velocity.x = 0.f;

	if (intersection.y != 0.f)
		this->velocity.y = 0.f;

	this->MovePosition(intersection);
}

void Demon::Attack()
{
	int current = this->animations[Actions::ATTACK].GetCurrent();
	sf::Vector2f dir, pos, other;

	dir = sf::Vector2f(ATTACK_VELOCITY, 0.f);
	sf::setAngle(dir, sf::angle(this->target->GetPosition(), this->GetPosition()));

	if (this->action == nullptr && ColisionManager::RayCastTarget(this, target, elapsedTime, dir))
	{
		this->looking_right = (this->target->GetPosition().x - this->GetPosition().x) >= 0.f;
		this->actionLenght = this->animations[Actions::ATTACK].GetDuration();
		this->next_ani = Actions::ATTACK;
		this->action = &Demon::Attack;
	}
	else if (this->next_ani == Actions::ATTACK)
	{
		this->velocity = dir;
		this->dashTimer = DASH_TIMER;
	}
}
void Demon::Dead()
{}
void Demon::Move()
{
	sf::Vector2f max(MAX_VELOCITY, MAX_VELOCITY);
	bool performing = this->action != nullptr;
	float acceleration = ACCELERATION;
	this->searchTimer += elapsedTime;

	if (this->action == &Demon::Attack)
		max = sf::Vector2f(ATTACK_VELOCITY, ATTACK_VELOCITY);

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

	if(!performing)
		this->next_ani = Actions::IDLE_WALK;

	switch (direction)
	{
	case DIRECTION::LEFT:
		this->velocity.x += -acceleration * elapsedTime * (1 - (2 * performing));
		this->looking_right = false;
		break;
	case DIRECTION::RIGHT:
		this->velocity.x += acceleration * elapsedTime * (1 - (2 * performing));
		this->looking_right = true;
		break;
	case DIRECTION::DOWN_LEFT:
		this->velocity += sf::Vector2f(-acceleration, acceleration) * elapsedTime * (1.f - (2.f * performing));
		this->looking_right = false;
		break;
	case DIRECTION::DOWN_RIGHT:
		this->velocity += sf::Vector2f(acceleration, acceleration) * elapsedTime * (1.f - (2.f * performing));
		this->looking_right = true;
		break;
	case DIRECTION::DOWN:
		this->velocity.y += acceleration * elapsedTime * (1 - (2 * performing));
		break;
	case DIRECTION::UP:
		this->velocity.y += -acceleration * elapsedTime * (1 - (2 * performing));
		break;
	case DIRECTION::UP_LEFT:
		this->velocity += sf::Vector2f(-acceleration, -acceleration) * elapsedTime * (1.f - (2.f * performing));
		this->looking_right = false;
		break;
	case DIRECTION::UP_RIGHT:
		this->velocity += sf::Vector2f(acceleration, -acceleration) * elapsedTime * (1.f - (2.f * performing));
		this->looking_right = true;
		break;
	default:
		if (this->velocity.x > 0.05f)
			this->velocity.x += -acceleration * elapsedTime;
		else if (this->velocity.x < -0.05f)
			this->velocity.x += acceleration * elapsedTime;

		if (this->velocity.y > 0.05f)
			this->velocity.y += -acceleration * elapsedTime;
		else if (this->velocity.y < -0.05f)
			this->velocity.y += acceleration * elapsedTime;
		break;
	}
	
	if (this->velocity.x > max.x)
		this->velocity.x = max.x;
	else if (this->velocity.x < -max.x)
		this->velocity.x = -max.x;

	if (this->velocity.y > max.y)
		this->velocity.y = max.y;
	else if (this->velocity.y < -max.y)
		this->velocity.y = -max.y;

	this->MovePosition(this->velocity);
}