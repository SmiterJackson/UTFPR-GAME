#pragma once

#include "ente/entity/entity.h"

namespace obstacle
{
	class Obstacle : public Entity
	{
	public:
		enum ObsType : unsigned short int
		{
			NEUTRAL,
			TOP,
			BOTTOM,
			LEFT,
			RIGHT,
			MIDDLE,
			LEFT_TOP_EDGE,
			RIGHT_TOP_EDGE,
			LEFT_BOTTOM_EDGE,
			RIGHT_BOTTOM_EDGE,
			INNER_LEFT_TOP,
			INNER_RIGHT_TOP,
			INNER_LEFT_BOTTOM,
			INNER_RIGHT_BOTTOM
		};

	protected:
		typedef std::vector<std::vector<ObsType>> TxtTypeMap;
		typedef std::unordered_map<ObsType, sf::IntRect> TextureMap;
		typedef std::unordered_multimap<ObsType, sf::Sprite> SpriteMap;

	public:
		Obstacle();
		Obstacle(const sf::Vector2f _size, const sf::Vector2f _position, const std::string _texturePath, const sf::IntRect neutral,
				 const TextureMap& _textureMap = TextureMap(), const TxtTypeMap& txtTypeMap = TxtTypeMap(), const float _scale = 1.0f, const bool _isStatic = true);
		virtual ~Obstacle();

		void SetHitBoxSize(const sf::Vector2f _size)
		{
			this->hitBox.setSize(_size);

			this->body.setTextureRect(sf::IntRect(0, 0, int(_size.x), int(_size.y)));
			this->body.setOrigin(_size / 2.f);
			this->body.setPosition(this->hitBox.getPosition());
		};

		void MovePosition(const float X_axis, const float Y_axis);
		void SetPosition(const float X_axis, const float Y_axis);

		virtual void SelfPrint();
		virtual void DebugSelfPrint();
		virtual void EntityColision(Entity* _other, const sf::Vector2f& intersection) = 0;
		virtual void MapColision(const sf::Vector2f& intersection) = 0;

	protected:
		virtual void TextureLoad(const std::vector<std::vector<ObsType>>& txtTypeMap, const sf::Vector2f& posRef, const std::string path);

	protected:
		static const sf::Vector2f& gridScale;
		bool showMain;

		SpriteMap sprites;
		TextureMap textureMap;
	};
}