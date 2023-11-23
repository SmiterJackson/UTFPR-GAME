#pragma once

#include "../entity.h"
#include "../auxiliary/traits/animated/animated.h"

namespace character
{
	class Character : public Entity, public trait::Animated
	{
	public:
		Character();
		Character(const Type _type, const sf::Vector2f _size, const sf::Vector2f _tokenSize, const std::string _texturePath,
			const AnimationSheet _animations = AnimationSheet(), const short int _lifeAmount = 3i16, 
			const float _invcDuration = 0.f, const bool _isStatic = true, const float _scale= 1.0f);
		virtual ~Character();

		const short int GetLifeCount() const { return this->life_counter; };
		virtual void Damage(unsigned short int damage = 1);

		virtual void Execute();
		virtual void DebugExecute();
		virtual void SelfPrint();
		virtual void DebugSelfPrint();

		virtual void EntityColision(Entity* _other, const sf::Vector2f& intersection) = 0;
		virtual void MapColision(const sf::Vector2f& intersection) = 0;

	protected:
		virtual void Attack() = 0;
		virtual void Dead() = 0;

	protected:
		static const float flickTimer;

		const float invcFrames;
		sf::Text invcText;

		float invcTimer;
		short int life_counter;
	};
}