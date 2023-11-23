#include "animated.h"
#include "../game.h"
using namespace trait;

Animated::Animation::Animation():
	sheet_token(), 
	offSet(),
	start(0), 
	end(0), 
	row(0), 
	current(0),
	timeAcumulator(0.f), 
	switchTime(1.f), 
	repeatable(true)
{};
Animated::Animation::Animation(int _start, int _end, int _row, sf::Vector2i size, 
	float animSwitchTime, bool repeatable, sf::Vector2f _offSet) :
	sheet_token(sf::Vector2i(0, 0), size),
	offSet(_offSet),
	start(_start), 
	end(_end), 
	row(_row), 
	current(_start),
	timeAcumulator(0.f), 
	switchTime(animSwitchTime), 
	repeatable(repeatable)
{};
Animated::Animation::Animation(int _start, int _end, int _row, int width, int height, 
	float animSwitchTime, bool repeatable, sf::Vector2f _offSet) :
	sheet_token(0, 0, width, height),
	offSet(_offSet),
	start(_start),
	end(_end),
	row(_row),
	current(_start),
	timeAcumulator(0.f),
	switchTime(animSwitchTime),
	repeatable(repeatable)
{};
Animated::Animation::~Animation()
{};

void Animated::Animation::ResetAnimation()
{
	this->current = this->start;
};
void Animated::Animation::Update(sf::Sprite& _bodyRef, sf::RectangleShape& _hitBoxRef, 
								const float& pElapsed_time, bool right)
{
	if (Game::GetState() == GameState::StateType::PAUSED || Game::GetState() == GameState::StateType::MENU)
		return;

	sf::IntRect token;
	sf::Vector2f off;
	this->timeAcumulator += pElapsed_time;

	if (this->timeAcumulator >= this->switchTime)
	{
		this->timeAcumulator -= this->switchTime;

		if (this->repeatable)
		{
			if (this->current == this->end)
				this->current = this->start;
		}

		if (this->current < this->end)
			this->current++;
	}

	token.top = this->row * this->sheet_token.height;
	token.height = this->sheet_token.height;
	
	/*if (right)
	{
		token.left = this->current * this->sheet_token.width;
		token.width = this->sheet_token.width;
	}
	else
	{
		token.left = (this->current + 1) * this->sheet_token.width;
		token.width = -this->sheet_token.width;
	}*/
	token.left = (this->current + !right) * this->sheet_token.width;
	token.width = this->sheet_token.width * (1 - (2 * !right));

	off = this->offSet * sf::Vector2i((1 - (2 * right)), 1);
	off += sf::Vector2i(sheet_token.width, sheet_token.height) / 2.f;

	_bodyRef.setTextureRect(token);
	_bodyRef.setOrigin(off);
	_bodyRef.setPosition(_hitBoxRef.getPosition());
};


Animated::Animated(sf::Sprite& _bodyRef, sf::RectangleShape& _hitBoxRef, const AnimationSheet _animations) :
	animations(),
	hitBoxRef(_hitBoxRef),
	bodyRef(_bodyRef),
	next_ani(0),
	last_ani(0),
	looking_right(true)
{
	AnimationSheet::const_iterator it;
	sf::Vector2f size;

	for (it = _animations.cbegin(); it != _animations.cend(); it++)
		this->animations.emplace(it->first, it->second);
};
Animated::~Animated()
{};

void Animated::UpdateAnimation(const float& pElapsedTime)
{
	if (this->next_ani != this->last_ani)
	{
		this->animations[this->last_ani].ResetAnimation();
		this->last_ani = this->next_ani;
	}
	
	this->animations[this->next_ani].Update(this->bodyRef, this->hitBoxRef, pElapsedTime, this->looking_right);
};

void Animated::AddAnimation(const int key, const Animation _animation)
{
	this->animations.emplace(key, _animation);
};
void Animated::AddAnimationRange(const AnimationSheet _animations)
{
	AnimationSheet::const_iterator it;
	for (it = _animations.cbegin(); it != _animations.cend(); it++)
		this->animations.emplace(it->first, it->second);
};
void Animated::RemoveAnimation(const int key)
{
	this->animations.erase(key);
};
void Animated::RemoveAnimationRange(const std::vector<int> keys)
{
	std::vector<int>::const_iterator it;
	for (it = keys.cbegin(); it != keys.cend(); it++)
		this->animations.erase(*it);
};