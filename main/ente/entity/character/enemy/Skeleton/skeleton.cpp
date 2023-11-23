#include "skeleton.h"
#include "stage/stage.h"
#include "manager/graphicManager/graphicManager.h"
using namespace manager;
using namespace character;
using namespace character::enemy;

#define PATH "resources/textures/sprite_sheet/skeleton.png"
#define TOKEN_SIZE sf::Vector2i(64, 64)

#define SEARCH_TIME 0.5f
#define DIST_TO_ATTACK sf::Vector2f(49.f, 42.f)

#define HITBOX_SIZE sf::Vector2f(26.f, 33.f)
#define ATTACK_HITBOX sf::Vector2f(46.f, 27.f)
#define JUMP_SPEED -5.f
#define ACCELERATION 8.f
#define DAMAGE 1

Skeleton::SkeletonAnimation Skeleton::classAnimations = {
	std::pair<int, Animation>(Actions::IDLE,	Animation(0, 3, 3,	TOKEN_SIZE, 0.27f, true)),
	std::pair<int, Animation>(Actions::WALK,	Animation(0, 11, 2, TOKEN_SIZE, 0.165f, true)),
	std::pair<int, Animation>(Actions::ATTACK,	Animation(0, 12, 0, TOKEN_SIZE, 0.13f, true)),
	std::pair<int, Animation>(Actions::DAMAGED,	Animation(0, 2, 4,	TOKEN_SIZE, 0.2f, true)),
	std::pair<int, Animation>(Actions::DIED,	Animation(0, 12, 1, TOKEN_SIZE, 0.2f, true))
};

Skeleton::Skeleton():
	Enemy(HITBOX_SIZE, sf::Vector2f(TOKEN_SIZE), PATH, classAnimations, 10, 1.f, 200.f, false, 1.f),
	ActionPerformer(),
	attackHitbox(),
	direction(DIRECTION::ERROR),
	onGround(false),
	searchTimer(0.f)
{
	attackHitbox.setFillColor(sf::Color::Transparent);
	attackHitbox.setOutlineThickness(-1.5f);
	attackHitbox.setSize(sf::Vector2f(0.f, 0.f));
	attackHitbox.setOrigin(ATTACK_HITBOX / 2.f);
};
Skeleton::~Skeleton()
{};

const std::list<Skeleton::Hitbox> Skeleton::GetHitBoxes()
{
	sf::Vector2f size(this->GetSize());
	std::list<Hitbox> hitboxes;

	hitboxes.emplace_back(this->GetPosition(), size, HitboxType::BODY);

	size = this->attackHitbox.getSize();
	if (size.x != 0 && size.y != 0)
		hitboxes.emplace_back(this->attackHitbox.getPosition(), size, HitboxType::ATTACK_BOX);

	return hitboxes;
}

void Skeleton::Execute()
{
	sf::Vector2f dist;

	this->Move();
	if (this->target != nullptr)
	{
		dist = sf::abs(this->target->GetPosition() - this->GetPosition());
		if (dist <= DIST_TO_ATTACK)
			this->Attack();
	}

	if (invcTimer > 0.f)
	{
		invcTimer -= elapsedTime;
		this->invcTimer *= !(this->invcTimer <= 0.f);
	}

	this->PerformAction(elapsedTime);
};
void Skeleton::DebugExecute()
{
	sf::Vector2f newPos, dist;
	std::stringstream ss;
	float round = 0.f;

	this->Move();
	if (this->target != nullptr)
	{
		dist = sf::abs(this->target->GetPosition() - this->GetPosition());
		if (dist <= DIST_TO_ATTACK)
			this->Attack();
	}

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
};
void Skeleton::DebugSelfPrint()
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

void Skeleton::EntityColision(Entity* _other, const sf::Vector2f& intersection)
{
	if (_other == nullptr)
		return;

	sf::Vector2f delta(intersection);
	sf::Vector2f size(this->attackHitbox.getSize());
	sf::Vector2f pos(this->attackHitbox.getPosition());

	sf::FloatRect rect(_other->GetBounds());
	sf::FloatRect atck(
		pos.x - size.x,
		pos.y - size.y,
		pos.x + size.x,
		pos.y + size.y
	);

	sf::FloatRect original = this->attackHitbox.getGlobalBounds();

	if (_other->GetType() == Type::OBSTACLE)
	{
		if (intersection.x != 0.f)
			this->velocity.x = 0.f;

		if (intersection.y != 0.f)
			this->velocity.y = *gravity * this->velocityCoeff * elapsedTime;

		if (intersection.y < 0.f)
			this->onGround = true;
	}
	else if(_other->GetType() == Type::PLAYER)
	{
		delta *= 0.f;
		this->velocity.x *= 0.f;

		if (ColisionManager::RectsOverlaping(this->GetBounds(), rect) ||
			ColisionManager::RectsOverlaping(atck, rect))
			static_cast<Character*>(_other)->Damage(DAMAGE);
	}

	this->MovePosition(delta);
};
void Skeleton::MapColision(const sf::Vector2f& intersection)
{
	if (intersection.x != 0.f)
		this->velocity.x = 0.f;

	if (intersection.y != 0.f)
		this->velocity.y = *gravity * this->velocityCoeff * elapsedTime;

	if(intersection.y < 0.f)
		this->onGround = true;

	this->MovePosition(intersection);
};

void Skeleton::Attack()
{
	int current = this->animations[Actions::ATTACK].GetCurrent();

	if(this->action == nullptr)
	{
		this->looking_right = (this->target->GetPosition().x - this->GetPosition().x) >= 0.f;
		this->actionLenght = this->animations[Actions::ATTACK].GetDuration();
		this->next_ani = Actions::ATTACK; 
		this->action = &Skeleton::Attack;
	}
	else if (this->next_ani == Actions::ATTACK && current >= 4 && current <= 9)
	{
		this->attackHitbox.setOutlineColor(
			this->pDebugFlagSub->GetDebugFlag() ?
			sf::Color::Red : sf::Color::Transparent
		);
		this->attackHitbox.setSize(ATTACK_HITBOX);
	}
	else
		this->attackHitbox.setSize(sf::Vector2f(0.f, 0.f));
};
void Skeleton::Dead()
{};
void Skeleton::Move()
{
	bool performing = this->action != nullptr;
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

	if(performing)
		direction = DIRECTION::ERROR;
	else if (direction == DIRECTION::UP			&& !onGround && !performing)
		direction = DIRECTION::ERROR;
	else if (direction == DIRECTION::UP_LEFT	&& !onGround && !performing)
		direction = DIRECTION::LEFT;
	else if (direction == DIRECTION::UP_RIGHT	&& !onGround && !performing)
		direction = DIRECTION::RIGHT;

	switch (direction)
	{
	case DIRECTION::LEFT:
		this->velocity += sf::Vector2f(-ACCELERATION * (1 - (2 * performing)), *gravity) * elapsedTime;
		this->next_ani = Actions::WALK;
		this->looking_right = false;
		break;
	case DIRECTION::RIGHT:
		this->velocity += sf::Vector2f(ACCELERATION * (1 - (2 * performing)), *gravity) * elapsedTime;
		this->next_ani = Actions::WALK;
		this->looking_right = true;
		break;
	case DIRECTION::DOWN_LEFT:
		this->velocity += sf::Vector2f(-ACCELERATION * (1 - (2 * performing)), *gravity) * elapsedTime;
		this->next_ani = Actions::WALK;
		this->looking_right = false;
		break;
	case DIRECTION::DOWN_RIGHT:
		this->velocity += sf::Vector2f(ACCELERATION * (1 - (2 * performing)), *gravity) * elapsedTime;
		this->next_ani = Actions::WALK;
		this->looking_right = true;
		break;
	case DIRECTION::DOWN:
		this->velocity += sf::Vector2f(0.f, *gravity) * elapsedTime;
		this->next_ani = Actions::IDLE;
		break;
	case DIRECTION::UP:
		this->onGround = false;
		this->velocity += sf::Vector2f(0.f, JUMP_SPEED);
		this->next_ani = Actions::IDLE;
		break;
	case DIRECTION::UP_LEFT:
		this->onGround = false;
		this->velocity += sf::Vector2f(-ACCELERATION * elapsedTime, JUMP_SPEED);
		this->next_ani = Actions::WALK;
		this->looking_right = false;
		break;
	case DIRECTION::UP_RIGHT:
		this->onGround = false;
		this->velocity += sf::Vector2f(ACCELERATION * elapsedTime, JUMP_SPEED);
		this->next_ani = Actions::WALK;
		this->looking_right = true;
		break;
	default:
		if(this->velocity.x > 0.05f)
			this->velocity += sf::Vector2f(-ACCELERATION, *gravity) * elapsedTime;
		else if(this->velocity.x < -0.05f)
			this->velocity += sf::Vector2f(ACCELERATION, *gravity) * elapsedTime;
		else
			this->velocity = sf::Vector2f(0.f, (*gravity * elapsedTime) + this->velocity.y);

		if(!performing)
			this->next_ani = Actions::IDLE;
		break;
	}

	if (this->velocity.x > 1.6f)
		this->velocity.x = 1.6f;
	else if (this->velocity.x < -1.6f)
		this->velocity.x = -1.6f;

	if (this->velocity.y > *maxGravity)
		this->velocity.y = *maxGravity;

	this->MovePosition(this->velocity);
	this->attackHitbox.setPosition(
		this->GetPosition() + 
		sf::Vector2f(
			8.f * (1 - (2 * !this->looking_right)), -8.f
		)
	);
};