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
			struct OffSet
			{
				OffSet(int _left = 0, int _top = 0, int _right = 0, int _bottom = 0, 
						sf::Vector2f _origin = sf::Vector2f(0.f, 0.f)):
					left(_left),
					top(_top),
					right(_right),
					bottom(_bottom),
					origin(_origin)
				{};

				int left	= 0;
				int top		= 0;
				int right	= 0;
				int bottom	= 0;

				const sf::Vector2f origin = sf::Vector2f(0.f, 0.f);
			};

		public:
			Animation();
			Animation(int _start, int _end, int _row, sf::Vector2i size, 
				float animSwitchTime, bool repeatable = false, OffSet _offSet = OffSet());
			Animation(int _start, int _end, int _row, int width, int height, 
				float animSwitchTime, bool repeatable = false, OffSet _offSet = OffSet());
			~Animation();

			float GetDuration() { return this->switchTime * (this->end - this->start + 1); };

			void ResetAnimation();
			void Update(sf::Sprite& _bodyRef, sf::RectangleShape& _hitBoxRef, const float& pElapsed_time, bool right = true);

		private:
			const sf::IntRect sheet_token;
			const OffSet offSet;

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