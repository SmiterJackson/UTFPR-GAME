#include "entity.h"
#include "../game.h"
using namespace manager;
using namespace stage;

#define ORIGIN_SIZE 1.f

EventManager::DebugFlagSubject* Entity::pDebugFlagSub = EventManager::DebugFlagSubject::GetInstance();

Entity::Entity() :
	Ente(Type::UNDEFINED, PrintPriority::undefined),
	velocityCoeff(1.f),
	isStatic(true),
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

	this->SetPosition(0.f, 0.f);

	this->originCircle.setFillColor(sf::Color::Transparent);
	this->originCircle.setRadius(ORIGIN_SIZE);
	this->originCircle.setOrigin(ORIGIN_SIZE / 2.f, ORIGIN_SIZE / 2.f);
	this->originCircle.setOutlineThickness(2.5f);
	this->originCircle.setOutlineColor(sf::Color::Blue);
};
Entity::Entity(const unsigned short int _type, const unsigned short int _printPriority, const sf::Vector2f _size,
	const sf::Vector2f _position, const std::string _texturePath, const sf::IntRect sheetCut,
	const bool _isStatic, const float _scale) :
	Ente(_type, _printPriority),
	velocityCoeff(1.f),
	isStatic(_isStatic), 
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
		this->texture = GraphicManager::LoadTexture(_texturePath, sheetCut);

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
	this->hitBox.setOutlineThickness(1.5f);

	this->originCircle.setFillColor(sf::Color::Transparent);
	this->originCircle.setRadius(ORIGIN_SIZE);
	this->originCircle.setOrigin(ORIGIN_SIZE / 2.f, ORIGIN_SIZE / 2.f);
	this->originCircle.setOutlineThickness(2.5f);
	this->originCircle.setOutlineColor(sf::Color::Blue);
};
Entity::~Entity()
{
	this->pDebugFlagSub->DettachObs(this);
};

const sf::Vector2f Entity::GetPosition() const
{
	return this->hitBox.getPosition();
}
void Entity::MovePosition(const float X_axis, const float Y_axis)
{
	this->hitBox.move(X_axis, Y_axis);
	this->body.move(X_axis, Y_axis);
}
void Entity::SetPosition(const float X_axis, const float Y_axis)
{
	this->hitBox.setPosition(X_axis, Y_axis);
	this->body.setPosition(X_axis, Y_axis);
}

void Entity::UpdateObs(const trait::Subject* alteredSub)
{
	if (alteredSub == nullptr || alteredSub != this->pDebugFlagSub)
		return;

	if(this->pDebugFlagSub->GetDebugFlag() == true)
		this->hitBox.setOutlineColor(sf::Color::Red);
	else
		this->hitBox.setOutlineColor(sf::Color::Transparent);
};

void Entity::Execute()
{
	this->Move();
};
void Entity::DebugExecute()
{
	sf::Vector2f newPos;

	this->Move();

	newPos = this->GetPosition();
	this->originCircle.setPosition(newPos.x, newPos.y);
};
void Entity::SetTexture(const std::string _texturePath, const sf::IntRect sheetCut)
{
	if (!_texturePath.empty())
	{
		this->texture = GraphicManager::LoadTexture(_texturePath, sheetCut);

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
};
void Entity::Move()
{
	float max = Stage::GetMaxGravityPull();
	this->velocity.y += this->velocityCoeff * elapsedTime * Stage::GetGravity();

	/*
	if (this->velocity.y > Stage::GetMaxGravityPull())
		this->velocity.y = Stage::GetMaxGravityPull();

	if (this->isStatic)
	{
		this->velocity.y = 0.f;
	}
	*/
	// versao branchless
	this->velocity.y = (
		(this->velocity.y * (this->velocity.y < max && !this->isStatic)) +
		(max * (this->velocity.y > max && !this->isStatic))
	);

	this->MovePosition(this->velocity.x, this->velocity.y);
};