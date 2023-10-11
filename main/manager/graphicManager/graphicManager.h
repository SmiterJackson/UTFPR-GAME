#pragma once

#include "../auxiliary/traits/state/gameState/gameState.h"

namespace sf
{
	typedef Rect<unsigned int> UIntRect;
}
namespace obstacle
{
	class Obstacle;
}
namespace character
{
	namespace enemy
	{
		class Enemy;
	}
}

namespace manager
{
	typedef std::unordered_map<std::string, sf::Texture*> TextureMap;

	class GraphicManager
	{
	public:
		static GraphicManager* GetInstance();
		static void DeconsInstance();

		static void SetCameraLimits(const sf::FloatRect limits) { cameraLim = limits; };

		const float GetZoom() const { return this->zoom; };
		void SetZoom(const float _zoom) { this->zoom = _zoom; };

		static const sf::FloatRect GetViewBounds()
		{
			return sf::FloatRect(
				view.getCenter().x - (view.getSize().x / 2.f),
				view.getCenter().y - (view.getSize().y / 2.f),
				view.getCenter().x + (view.getSize().x / 2.f),
				view.getCenter().y + (view.getSize().y / 2.f)
			);
		};
		static const sf::Vector2f GetViewPosition()	{ return view.getCenter(); };
		static const sf::Vector2f GetViewSize()	{ return view.getSize(); };

		static const bool IsWindowOpen() { return window->isOpen(); };
		static void CloseWindow() { window->close(); };
		static sf::RenderWindow& GetWindow() { return *window; };

		static const sf::Vector2f MouseToView()
		{
			return window->mapPixelToCoords(sf::Mouse::getPosition(*window));
		};
		static const sf::Vector2i MouseToGrid()
		{
			sf::Vector2f mousToView = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
			return sf::Vector2i(
				int(mousToView.x / gridScale.x),
				int(mousToView.y / gridScale.y)
			);
		};
		static const sf::UIntRect GridIndexsToCamera()
		{
			sf::FloatRect bounds(GraphicManager::GetViewBounds());
			sf::Vector2f grid = gridScale;

			sf::UIntRect indexs(
				unsigned int(bounds.left	/ grid.x - (bounds.left   > 0)),
				unsigned int(bounds.top		/ grid.y - (bounds.top    > 0)),
				unsigned int(bounds.width	/ grid.x + (bounds.width  < cameraLim.width)),
				unsigned int(bounds.height	/ grid.y + (bounds.height < cameraLim.height))
			);

			return indexs;
		};
		const void SetGameState(trait::GameState* _state) { this->pState = _state; };

		void SetResizeDistortion(const bool X_axis, const bool Y_axis)
		{
			this->distort_x = X_axis;
			if (!this->distort_x && this->bar_x)
				this->bar_x = false;

			this->distort_y = Y_axis;
			if (!this->distort_y && this->bar_y)
				this->bar_y = false;
		};
		void SetResizeBar(const bool X_axis, const bool Y_axis)
		{
			this->bar_x = X_axis;
			if (this->bar_x && !this->distort_x)
				this->distort_x = true;

			this->bar_y = Y_axis;
			if (this->bar_y && !this->distort_y)
				this->distort_y = true;
		};

		static void SetGridScale(const sf::Vector2f _scale) { gridScale = _scale; };

		static void UpdateCamera();
		void WindowResize() const;

		static void Draw(const sf::RectangleShape& drawTarget);
		static void Draw(const sf::CircleShape& drawTarget);
		static void Draw(const sf::Sprite& drawTarget);
		static void Draw(const sf::Text& drawTarget);

		static const std::list<obstacle::Obstacle*> GetCameraEntities(const std::list<obstacle::Obstacle*>& entities);
		static const std::list<character::enemy::Enemy*> GetCameraEntities(const std::vector<character::enemy::Enemy*>& entities);

		static sf::Font* GetFont() { return font; };
		static sf::Texture* LoadTexture(std::string texturePath, sf::IntRect sheetCut = sf::IntRect(0, 0, 0, 0), bool repeated = false);

		void Update();

	private:
		GraphicManager();
		~GraphicManager();

	private:
		static const float cameraExtraSpace;
		static trait::GameState* pState;
		static GraphicManager* instance;
		static TextureMap* textures;
		static sf::Font* font;
		static sf::RenderWindow* window;
		static sf::FloatRect cameraLim;
		static sf::View view;
		static sf::Vector2f gridScale;

		const sf::Vector2f originalSize;

		float zoom;
		bool distort_x, distort_y;
		bool bar_x, bar_y;

	};
}