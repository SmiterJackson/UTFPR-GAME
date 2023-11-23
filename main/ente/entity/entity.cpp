#include "entity.h"
#include "../game.h"
using namespace manager;
using namespace stage;

#define ORIGIN_SIZE 1.f

EventManager::DebugFlagSubject* Entity::pDebugFlagSub = EventManager::DebugFlagSubject::GetInstance();
Stage* Entity::pStage = nullptr;
float* Entity::maxGravity = nullptr;
float* Entity::gravity = nullptr;

Entity::Entity() :
	Ente(Type::UNDEFINED, PrintPriority::undefined),
	isStatic(true),
	velocityCoeff(1.f),
	hitBox(),
	texture(nullptr),
	body(),
	velocity(0.f, 0.f),
	originCircle()
{
	if (pDebugFlagSub == nullptr)
		pDebugFlagSub = EventManager::DebugFlagSubject::GetInstance();

	this->pDebugFlagSub->AttachObs(this);

	this->hitBox.setTexture(NULL);
	this->hitBox.setFillColor(sf::Color::Transparent);
	this->hitBox.setOutlineThickness(1.5f);
	this->hitBox.setOutlineColor(sf::Color::Green);

	this->SetPosition(sf::Vector2f(0.f, 0.f));

	this->originCircle.setFillColor(sf::Color::Transparent);
	this->originCircle.setRadius(ORIGIN_SIZE);
	this->originCircle.setOrigin(ORIGIN_SIZE / 2.f, ORIGIN_SIZE / 2.f);
	this->originCircle.setOutlineThickness(2.5f);
	this->originCircle.setOutlineColor(sf::Color::Blue);
}
Entity::Entity(const Type _type, const PrintPriority _priority, const sf::Vector2f _size,
	const sf::Vector2f _position, const std::string _texturePath, const sf::IntRect sheetCut,
	const bool _isStatic, const float _scale) :
	Ente(_type, _priority),
	isStatic(_isStatic),
	velocityCoeff(1.f),
	hitBox(), 
	texture(nullptr), 
	body(),
	velocity(0.f, 0.f), 
	originCircle()
{
	if (pDebugFlagSub == nullptr)
		pDebugFlagSub = EventManager::DebugFlagSubject::GetInstance();

	this->pDebugFlagSub->AttachObs(this);

	if (!_texturePath.empty())
	{
		this->texture = pGraphicManager->LoadTexture(_texturePath, sheetCut);

		if (this->texture != nullptr)
		{
			this->body.setTexture(*this->texture);
			this->texture->setRepeated(false);

			this->body.setOrigin(
				this->texture->getSize().x / 2.f,
				this->texture->getSize().y / 2.f
			);
		}
	}
	this->body.setScale(_scale, _scale);
	this->body.setPosition(_position);

	this->hitBox.setTexture(NULL);
	this->hitBox.setFillColor(sf::Color::Transparent);
	this->hitBox.setSize(_size);
	this->hitBox.setPosition(_position);
	this->hitBox.setScale(_scale, _scale);
	this->hitBox.setOrigin(this->hitBox.getSize() / 2.f);
	this->hitBox.setOutlineThickness(-1.5f);
	this->hitBox.setOutlineColor(sf::Color::Green);

	this->originCircle.setFillColor(sf::Color::Transparent);
	this->originCircle.setRadius(ORIGIN_SIZE);
	this->originCircle.setOrigin(ORIGIN_SIZE / 2.f, ORIGIN_SIZE / 2.f);
	this->originCircle.setOutlineThickness(2.5f);
	this->originCircle.setOutlineColor(sf::Color::Blue);
}
Entity::~Entity()
{
	this->pDebugFlagSub->DettachObs(this);
}

void Entity::UpdateObs(const trait::Subject* alteredSub)
{
	if (alteredSub == nullptr || alteredSub != this->pDebugFlagSub)
		return;

	if(this->pDebugFlagSub->GetDebugFlag() == true)
		this->hitBox.setOutlineColor(sf::Color::Green);
	else
		this->hitBox.setOutlineColor(sf::Color::Transparent);
}
const std::list<Entity::Hitbox> Entity::GetHitBoxes()
{
	sf::Vector2f size(this->GetSize());
	std::list<Hitbox> hitboxes;

	hitboxes.emplace_back(this->GetPosition(), size, HitboxType::BODY);

	return hitboxes;
}

void Entity::SetTexture(const std::string _texturePath, const sf::IntRect sheetCut)
{
	if (!_texturePath.empty())
	{
		this->texture = pGraphicManager->LoadTexture(_texturePath, sheetCut);

		if (this->texture != nullptr)
		{
			this->body.setTexture(*this->texture);
			this->texture->setRepeated(false);

			this->body.setOrigin(
				this->texture->getSize().x / 2.f,
				this->texture->getSize().y / 2.f
			);
		}
	}
}

void Entity::Execute()
{
	this->Move();
}
void Entity::DebugExecute()
{
	sf::Vector2f newPos;

	this->Move();

	newPos = this->GetPosition();
	this->originCircle.setPosition(newPos.x, newPos.y);
}

void Entity::Move()
{
	if (maxGravity == nullptr || gravity == nullptr)
		return;

	this->velocity.y += this->velocityCoeff * elapsedTime * *gravity;

	/*
	if (this->velocity.y > *maxGravity)
		this->velocity.y = *maxGravity;

	if (this->isStatic)
	{
		this->velocity.y = 0.f;
	}
	*/
	// versao branchless
	this->velocity.y = (
		(this->velocity.y * (this->velocity.y < *maxGravity && !this->isStatic)) +
		(*maxGravity * (this->velocity.y > *maxGravity && !this->isStatic))
	);

	this->MovePosition(this->velocity);
}