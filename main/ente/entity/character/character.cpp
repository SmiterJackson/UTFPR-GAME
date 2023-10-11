#include "character.h"
#include "manager/graphicManager/graphicManager.h"
using namespace character;
using namespace manager;

#define DAMAGE_FLICK_TIMER 0.1f

Character::Character() :
	Entity(
		Type::UNDEFINED, PrintPriority::characters, sf::Vector2f(0.f, 0.f),
		sf::Vector2f(0.f, 0.f), std::string(""), sf::IntRect(0, 0, 0, 0)
	),
	Animated(this->body, this->hitBox),
	invcFrames(0.3f),
	invecTimer(0.f), 
	deathTimer(1.f), 
	damagedTimer(DAMAGE_FLICK_TIMER), 
	life_counter(5U)
{};
Character::Character(const unsigned short int _type, const sf::Vector2f _size, const sf::Vector2f _tokenSize,
	const std::string _texturePath, const AnimationSheet _animations, const unsigned int _lifeAmount, 
	const float _invcDuration,	const float _deathTime, const bool _isStatic, const float _scale) :
	Entity(
		_type, PrintPriority::characters, _size, sf::Vector2f(0.f, 0.f),
		_texturePath, sf::IntRect(0, 0, 0, 0), _isStatic, _scale
	),
	Animated(this->body, this->hitBox, _animations),
	invcFrames(_invcDuration),
	invecTimer(0.f),
	deathTimer(_deathTime),
	damagedTimer(DAMAGE_FLICK_TIMER),
	life_counter(_lifeAmount)
{
	this->body.setOrigin(_tokenSize /2.f);
	this->body.setPosition(0.f, 0.f);
};
Character::~Character()
{};

void Character::Execute()
{
	if (invecTimer > 0.f)
		invecTimer -= elapsedTime;

	this->Move();
};
void Character::DebugExecute()
{
	sf::Vector2f newPos;

	if (invecTimer > 0.f)
		invecTimer -= elapsedTime;

	this->Move();

	newPos = this->GetPosition();
	this->originCircle.setPosition(newPos.x, newPos.y);
};
void Character::SelfPrint()
{
	this->UpdateAnimation(elapsedTime);
	sf::IntRect _body = body.getTextureRect();

	if (invecTimer > 0.f && int(invecTimer / DAMAGE_FLICK_TIMER) % 2 == 0)
		body.setTextureRect(sf::IntRect(0, 0, 0, 0));

	GraphicManager::Draw(this->body);
	body.setTextureRect(_body);
};
void Character::DebugSelfPrint()
{
	this->UpdateAnimation(elapsedTime);
	sf::IntRect _body = body.getTextureRect();

	if (invecTimer > 0.f && int(invecTimer / DAMAGE_FLICK_TIMER) % 2 == 0)
		body.setTextureRect(sf::IntRect(0, 0, 0, 0));

	GraphicManager::Draw(this->body);
	GraphicManager::Draw(this->hitBox);
	GraphicManager::Draw(this->originCircle);
	body.setTextureRect(_body);
};