#pragma once

#include "../entity.h"
#include "../auxiliary/traits/animated/animated.h"

namespace character
{
	class Character : public Entity, public trait::Animated
	{
	public:
		Character();
		Character(const unsigned short int _type, const sf::Vector2f _size, const sf::Vector2f _tokenSize, const std::string _texturePath,
			const AnimationSheet _animations = AnimationSheet(), const unsigned int _lifeAmount = 0U, const float _invcDuration = 0.f,
			const float _deathTime = 1.f, const bool _isStatic = true, const float _scale= 1.0f);
		virtual ~Character();

		const unsigned int GetLifeCount() const { return this->life_counter; };
		virtual void Damaged()
		{
			if (this->invecTimer <= 0.f)
			{
				this->life_counter--;
				this->invecTimer = this->invcFrames;
			}

			if (this->life_counter <= 0)
			{
				this->Died();
				return;
			}
		};

		virtual void Execute();
		virtual void DebugExecute();
		virtual void SelfPrint();
		virtual void DebugSelfPrint();

		virtual void EntityColision(Entity* _other, const sf::Vector2f& intersection) = 0;
		virtual void MapColision(const sf::Vector2f& intersection) = 0;

	protected:
		virtual void Attack() = 0;
		virtual void Died() = 0;

	protected:
		const float invcFrames;

		float invecTimer;
		float deathTimer;
		float damagedTimer;
		unsigned int life_counter;
	};
}