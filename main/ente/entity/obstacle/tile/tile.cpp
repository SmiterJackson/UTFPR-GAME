#include "tile.h"
using namespace obstacle;

#define PATH "resources/textures/tile_sheet/tile_sheet.png"

#define WIDTH  32
#define HEIGHT 32

#define RECT(x, y) sf::IntRect(WIDTH * x, WIDTH * y, WIDTH, WIDTH)
#define VALUE(key, value) Obstacle::TextureMap::value_type(key, value)

Obstacle::TextureMap Tile::map = {
	VALUE(ObsType::NEUTRAL,				RECT(3, 0)),
	VALUE(ObsType::LEFT_TOP_EDGE,		RECT(0, 0)),
	VALUE(ObsType::TOP,					RECT(1, 0)),
	VALUE(ObsType::RIGHT_TOP_EDGE,		RECT(2, 0)),
	VALUE(ObsType::LEFT,				RECT(0, 1)),
	VALUE(ObsType::MIDDLE,				RECT(1, 1)),
	VALUE(ObsType::RIGHT,				RECT(2, 1)),
	VALUE(ObsType::LEFT_BOTTOM_EDGE,	RECT(0, 2)),
	VALUE(ObsType::BOTTOM,				RECT(1, 2)),
	VALUE(ObsType::RIGHT_BOTTOM_EDGE,	RECT(2, 2)),
	VALUE(ObsType::INNER_LEFT_TOP,		RECT(2, 5)),
	VALUE(ObsType::INNER_RIGHT_TOP,		RECT(0, 5)),
	VALUE(ObsType::INNER_LEFT_BOTTOM,	RECT(2, 3)),
	VALUE(ObsType::INNER_RIGHT_BOTTOM,	RECT(0, 3))
};

Tile::Tile() :
	Obstacle(
		sf::Vector2f(float(1 * WIDTH), float(1 * HEIGHT)), sf::Vector2f(0.f,0.f), 
		PATH, map[ObsType::NEUTRAL], map, TxtTypeMap(), 1.f, true
	)
{};
Tile::Tile(const sf::Vector2u _size, const sf::Vector2f _position, const TxtTypeMap& txtTypeMap):
	Obstacle(
		sf::Vector2f(float(_size.x * WIDTH), float(_size.y * HEIGHT)), 
		_position, PATH, map[ObsType::NEUTRAL], map, txtTypeMap, 1.f, true
	)
{};
Tile::~Tile()
{};

void Tile::EntityColision(Entity* _other, const sf::Vector2f& intersection)
{
	if (_other == nullptr)
		return;

	_other->SetVelocityCoeff(1.f);
};
void Tile::MapColision(const sf::Vector2f& intersection)
{};