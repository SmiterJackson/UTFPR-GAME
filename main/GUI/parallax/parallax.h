#pragma once

#include "ente/ente.h"

namespace GUI 
{
	typedef std::pair<sf::Texture*, std::vector<sf::Sprite>> Layer;
	class Parallax : public Ente
	{
	public:
		Parallax(const std::vector<std::string>& paths = std::vector<std::string>(), const float _scale = 1.f);
		~Parallax();
		Parallax(const Parallax& other) = delete;

		void SetBackgrounds(const std::vector<std::string>& paths);
		void SetSizeCoefficient(float _scale);

		void ResetBackground();

		void Execute();
		void DebugExecute();
		void SelfPrint();
		void DebugSelfPrint();

		void Print();
		void Update();

	private:
		std::vector<Layer> backGrounds;
		sf::Vector2f lastPos;

		float layers_spd;
		float scale;
	};
}