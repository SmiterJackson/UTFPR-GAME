#pragma once

#include <iostream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <limits>
#include <concepts>
#include <type_traits>
#include <stdarg.h>
#include <numbers>

#include <stack>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>

#include "SFML\Graphics.hpp"

#define PI std::numbers::pi_v<long double>

// templates para extrair parametro de templates definidos
namespace exct
{
	template <typename T>
	struct temp_param;

	template <template <typename ...> class C, typename T>
	struct temp_param<C<T>>
	{
		using type = T;
	};

	template <typename T>
	using temp_param_t = typename temp_param<T>::type;
}

namespace sf
{
	typedef Rect<unsigned int> UIntRect;

	template<typename T>
	concept derived = std::derived_from<T, sf::Vector2<exct::temp_param_t<T>>>;

	template<class T>
		requires derived<T>
	const T abs(const T& vec)
	{
		return T(std::abs(vec.x), std::abs(vec.y));
	}
	template<class T>
		requires derived<T>
	const double norm(const T& vec)
	{
		return std::sqrt(std::pow(vec.x, 2) + std::pow(vec.y, 2));
	}
	template<class T>
		requires derived<T>
	const double internalProduct(const T& lVec, const T& rVec)
	{
		return (lVec.x * rVec.x) + (lVec.y * rVec.y);
	}
	template<class T>
		requires derived<T>
	const long double angle(const T& lVec, const T& rVec)
	{
		// calcula o ângulo do vertor esquerdo com relação ao direito sendo o direito a origem (em detrimento das coordenadas no game)
		long double size = 0.0, angle = 0.0;
		T fromOther = lVec - rVec;

		size = norm(fromOther);
		angle = std::acosl(fromOther.x / size) * 180.0 / PI;

		if (std::isnan(angle))
			return 0.0;

		if (fromOther.y > 0.0)
			angle += (180.0 - angle) * 2.0;

		return angle;
	}
	template<class T>
		requires derived<T>
	const void setAngle(T& lVec, long double angle)
	{
		long double cos = 0.0, sin = 0.0, radian = (angle * PI / 180.0L);
		cos = std::round(std::cosl(radian) * 10000.L) / 10000.L;
		sin = std::round(std::sinl(radian) * 10000.L) / 10000.L;
		
		lVec = T(
			float((cos * lVec.x) - (sin * lVec.y)),
			float((sin * lVec.x) + (cos * lVec.y))
		);

		// como ajusta o angulo em coordenadas usuais inverte-se a direção no eixo y em detrimento da inversão usada na biblioteca SFML
		lVec.y = -lVec.y;
	}

	inline sf::Vector2f operator* (const sf::Vector2f& l, const sf::Vector2f& r)
	{
		return sf::Vector2f(
			l.x * r.x, l.y * r.y
		);
	}
	inline sf::Vector2f operator* (const sf::Vector2f& l, const sf::Vector2i& r)
	{
		return sf::Vector2f(
			l.x * r.x, l.y * r.y
		);
	}
	inline sf::Vector2f operator* (const float& l, const sf::Vector2<short>& r)
	{
		return sf::Vector2f(
			l * r.x, l * r.y
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
	inline sf::Vector2f operator/ (const sf::Vector2i& l, const float& r)
	{
		return sf::Vector2f(
			l.x / r, l.y / r
		);
	}
	inline bool operator<= (const sf::Vector2f& l, const sf::Vector2f& r)
	{
		return (l.x <= r.x && l.y <= r.y);
	}
	inline bool operator>= (const sf::Vector2f& l, const sf::Vector2f& r)
	{
		return (l.x >= r.x && l.y >= r.y);
	}
}