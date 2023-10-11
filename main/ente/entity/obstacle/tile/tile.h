#pragma once

#include "../obstacle.h"

namespace obstacle
{
	class Tile : public Obstacle
	{
	public:
		enum TileType : unsigned short int
		{
			CASTLE,
			SEWER,
			FLESH
		};

	public:
		Tile();
		Tile(const sf::Vector2u	_size, const sf::Vector2f _position, const TxtTypeMap& txtTypeMap);
		virtual ~Tile();

		virtual void EntityColision(Entity* _other, const sf::Vector2f& intersection);
		virtual void MapColision(const sf::Vector2f& intersection);

	protected:
		static TextureMap map;
	};
}