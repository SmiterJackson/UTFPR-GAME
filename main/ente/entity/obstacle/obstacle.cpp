#include "obstacle.h"
#include "manager/graphicManager/graphicManager.h"
#include "stage/stage.h"
using namespace obstacle;
using namespace manager;
using namespace stage;

const sf::Vector2f& Obstacle::gridScale(Stage::GetGridScale());

Obstacle::Obstacle() :
	Entity(),
	showMain(true),
	sprites(),
	textureMap()
{};
Obstacle::Obstacle(const sf::Vector2f _size, const sf::Vector2f _position, const std::string _texturePath, const sf::IntRect neutral, 
				   const TextureMap& _textureMap, const TxtTypeMap& txtTypeMap, const float _scale, const bool _isStatic) :
	Entity(
		Type::OBSTACLE, PrintPriority::obstacles, _size, _position, 
		_texturePath, neutral, _isStatic, _scale
	),
	showMain(true),
	sprites(),
	textureMap(_textureMap)
{
	if (this->texture != nullptr)
		this->texture->setRepeated(true);

	this->TextureLoad(txtTypeMap, _position, _texturePath);
	this->hitBox.move(_size / 2.f);
};
Obstacle::~Obstacle()
{};

void Obstacle::TextureLoad(const std::vector<std::vector<ObsType>>& txtTypeMap, const sf::Vector2f& posRef, const std::string path)
{
	if (txtTypeMap.size() < 1)
		return;
	if (txtTypeMap[0].size() < 1)
		return;

	sf::IntRect range(0, 0, txtTypeMap.size(), txtTypeMap[0].size());
	std::list<sf::Vector2i> ignoreList;
	SpriteMap::iterator it;

	sf::Vector2i size(0, 0);

	int i = 0, j = 0, x = 0, y = 0;
	ObsType type = NEUTRAL;

	this->showMain = false;

	auto distance = [&](const unsigned int& x, const unsigned int& y)->sf::Vector2f
	{
		return sf::Vector2f(
			(gridScale.x * x) + posRef.x,
			(gridScale.y * y) + posRef.y
		);
	};

	for (i = range.left; i < range.width; i++)
	{
		for (j = 0; j < range.height; j++)
		{
			ignoreList.remove_if(
				[&](const sf::Vector2i& val) -> bool
				{
					return val.x < i || (val.x == i && val.y < j);
				}
			);

			for (sf::Vector2i& vec : ignoreList)
				if (vec.x == i && vec.y == j)
					goto OFF_MAJOR;

			type = txtTypeMap[i][j];
			range.left = i;
			range.top = j;
			x = i;
			y = j;

			while (x < range.width && txtTypeMap[x][j] == type)
				x++;
			range.width = x;

			while (y < range.height && txtTypeMap[i][y] == type)
			{
				y++;

				for (sf::Vector2i& vec : ignoreList)
					if (vec.x == i && vec.y == y)
						goto OFF_MINOR;
			}
		OFF_MINOR:;
			range.height = y;

			for (x = i + 1; x < range.width; x++)
				for (y = j + 1; y < range.height; y++)
					if (txtTypeMap[x][y] != type)
						range.height = y;

			for (x = range.left + 1; x < range.width; x++)
				for (y = range.top; y < range.height; y++)
					ignoreList.emplace_back(x, y);

			size = sf::Vector2i(
				int((range.width - range.left) * gridScale.x),
				int((range.height - range.top) * gridScale.y)
			);

			sprites.emplace(type, sf::Sprite());
			it = --sprites.equal_range(type).second;

			it->second.setTexture(
				*(pGraphicManager->LoadTexture(path, textureMap[type], true))
			);

			it->second.setTextureRect(sf::IntRect(0, 0, size.x, size.y));
			it->second.setPosition(distance(i, j));

			j = range.height - 1;

			range.width = txtTypeMap.size();
			range.height = txtTypeMap[0].size();
		OFF_MAJOR:;
		}
	}
};

void Obstacle::MovePosition(const float X_axis, const float Y_axis)
{
	SpriteMap::iterator spr;

	this->hitBox.move(X_axis, Y_axis);
	this->body.move(X_axis, Y_axis);

	for (spr = sprites.begin(); spr != sprites.end(); spr++)
		spr->second.move(X_axis, Y_axis);
}
void Obstacle::SetPosition(const float X_axis, const float Y_axis)
{
	this->hitBox.setPosition(X_axis, Y_axis);
	this->body.setPosition(X_axis, Y_axis);
}

void Obstacle::SelfPrint()
{
	SpriteMap::iterator spr;

	if(this->showMain) pGraphicManager->Draw(this->body);

	for (spr = sprites.begin(); spr != sprites.end(); spr++)
		pGraphicManager->Draw(spr->second);
};
void Obstacle::DebugSelfPrint()
{
	SpriteMap::iterator spr;

	if (this->showMain) pGraphicManager->Draw(this->body);

	for (spr = sprites.begin(); spr != sprites.end(); spr++)
		pGraphicManager->Draw(spr->second);

	pGraphicManager->Draw(this->hitBox);
	pGraphicManager->Draw(this->originCircle);
};