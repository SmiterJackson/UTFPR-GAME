#include "character.h"
#include "manager/graphicManager/graphicManager.h"
using namespace character;
using namespace manager;

const float Character::flickTimer = 0.1f;

Character::Character() :
	Entity(
		Type::UNDEFINED, PrintPriority::characters, sf::Vector2f(0.f, 0.f),
		sf::Vector2f(0.f, 0.f), std::string(""), sf::IntRect(0, 0, 0, 0)
	),
	Animated(this->body, this->hitBox),
	invcFrames(0.3f),
	invcText(),
	invcTimer(0.f),
	life_counter(5i16)
{};
Character::Character(const Type _type, const sf::Vector2f _size, const sf::Vector2f _tokenSize,
	const std::string _texturePath, const AnimationSheet _animations, const short int _lifeAmount, 
	const float _invcDuration, const bool _isStatic, const float _scale) :
	Entity(
		_type, PrintPriority::characters, _size, sf::Vector2f(0.f, 0.f),
		_texturePath, sf::IntRect(0, 0, 0, 0), _isStatic, _scale
	),
	Animated(this->body, this->hitBox, _animations),
	invcFrames(_invcDuration),
	invcText(),
	invcTimer(0.f),
	life_counter(_lifeAmount)
{
	this->invcText.setFont(*(pGraphicManager->GetFont()));
	this->invcText.setCharacterSize(14);
	this->invcText.setString("0.0s");

	this->body.setOrigin(_tokenSize / 2.f);
	this->body.setPosition(0.f, 0.f);
};
Character::~Character()
{};

void Character::Damage(unsigned short int damage)
{
	if (this->invcTimer <= 0.f)
	{
		this->life_counter -= damage;
		this->invcTimer = this->invcFrames;
	}
}

void Character::Execute()
{
	this->Move();

	if (invcTimer > 0.f)
	{
		invcTimer -= elapsedTime;
		this->invcTimer *= !(this->invcTimer <= 0.f);
	}
};
void Character::DebugExecute()
{
	sf::Vector2f newPos;
	std::stringstream ss;
	float round = 0.f;

	this->Move();

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
void Character::SelfPrint()
{
	this->UpdateAnimation(elapsedTime);
	sf::IntRect _body = body.getTextureRect();

	if (invcTimer > 0.f && int(invcTimer / flickTimer) % 2 == 0)
		body.setTextureRect(sf::IntRect(0, 0, 0, 0));

	pGraphicManager->Draw(this->body);
	body.setTextureRect(_body);
};
void Character::DebugSelfPrint()
{
	this->UpdateAnimation(elapsedTime);
	sf::IntRect _body = body.getTextureRect();

	if (invcTimer > 0.f && int(invcTimer / flickTimer) % 2 == 0)
		body.setTextureRect(sf::IntRect(0, 0, 0, 0));

	pGraphicManager->Draw(this->body);
	pGraphicManager->Draw(this->hitBox);
	pGraphicManager->Draw(this->originCircle);
	pGraphicManager->Draw(this->invcText);
	body.setTextureRect(_body);
};