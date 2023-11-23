#pragma once

#include "stdafx.h"

namespace trait
{
	class Animated
	{
	public:
		class Animation
		{
		public:
			Animation();
			Animation(int _start, int _end, int _row, sf::Vector2i size, 
				float animSwitchTime, bool repeatable = false, sf::Vector2f _offSet = sf::Vector2f(0.f, 0.f));
			Animation(int _start, int _end, int _row, int width, int height, 
				float animSwitchTime, bool repeatable = false, sf::Vector2f _offSet = sf::Vector2f(0.f, 0.f));
			~Animation();

			const float GetDuration() const { return this->switchTime * (this->end - this->start + 1); };
			const int GetCurrent() const { return this->current; };

			void ResetAnimation();
			void Update(sf::Sprite& _bodyRef, sf::RectangleShape& _hitBoxRef, const float& pElapsed_time, bool right = true);

		private:
			const sf::IntRect sheet_token;
			const sf::Vector2f offSet;

			int start, end, row, current;
			float timeAcumulator, switchTime;
			bool repeatable;
		};
		typedef std::vector<std::pair<int, Animation>> AnimationSheet;

		Animated(sf::Sprite& _bodyRef, sf::RectangleShape& _hitBoxRef, 
			const AnimationSheet _animations = AnimationSheet());
		virtual ~Animated();

		void InvertDirection() { this->looking_right = !this->looking_right; };

		void UpdateAnimation(const float& pElapsedTime);

		void AddAnimation(const int key, const Animation _animation);
		void AddAnimationRange(const AnimationSheet _animations);
		void RemoveAnimation(const int key);
		void RemoveAnimationRange(const std::vector<int> keys);

	protected:
		std::unordered_map<int, Animation> animations;
		sf::RectangleShape& hitBoxRef;
		sf::Sprite& bodyRef;

		unsigned int next_ani, last_ani;
		bool looking_right;
	};
}