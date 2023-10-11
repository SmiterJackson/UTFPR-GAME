#pragma once

#include <iostream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <limits>
#include <stdarg.h>

#include <stack>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>

#include "SFML\Graphics.hpp"

namespace sf
{
	inline sf::Vector2f operator* (const sf::Vector2f& l, const sf::Vector2f& r)
	{
		return sf::Vector2f(
			l.x * r.x, l.y * r.y
		);
	}
	inline sf::Vector2f operator/ (const sf::Vector2f& l, const sf::Vector2f& r)
	{
		return sf::Vector2f(
			l.x / r.x, l.y / r.y
		);
	}
	inline sf::Vector2f operator/ (const float& l, const sf::Vector2f& r)
	{
		return sf::Vector2f(
			l / r.x, l / r.y
		);
	}
	inline sf::Vector2f operator* (const float& l, const sf::Vector2<short>& r)
	{
		return sf::Vector2f(
			l * r.x, l * r.y
		);
	}
}