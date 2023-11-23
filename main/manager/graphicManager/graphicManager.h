#pragma once

#include "../auxiliary/traits/state/gameState/gameState.h"
#include "../auxiliary/traits/subject/subject.h"
#include "manager/eventManager/eventManager.h"

const enum PrintPriority : const unsigned short int
{
	undefined = 0,
	background,
	obstacles,
	characters,
	projectiles,
	interfaces,
	buttons
};

class Entity;

namespace manager
{
	typedef std::unordered_map<std::string, sf::Texture*> TextureMap;

	class GraphicManager : public trait::Observer
	{
	public:
		static GraphicManager* GetInstance();
		static void DeconsInstance();

		void SetCameraLimits(const sf::FloatRect limits) { cameraLim = limits; }

		const float GetZoom() const { return this->zoom; }
		void SetZoom(const float _zoom) { this->zoom = _zoom; }

		const sf::FloatRect GetViewBounds()
		{
			sf::Vector2f center(view.getCenter());
			sf::Vector2f size(view.getSize() / 2.f);
			return sf::FloatRect(
				center.x - size.x,
				center.y - size.y,
				center.x + size.x,
				center.y + size.y
			);
		}
		const sf::Vector2f GetViewPosition() { return view.getCenter(); }
		const sf::Vector2f GetViewSize() { return view.getSize(); }

		sf::RenderWindow& GetWindow() { return window; }
		const bool IsWindowOpen() { return window.isOpen(); }
		void CloseWindow() { window.close(); }

		const sf::Vector2f MouseToView()
		{
			return window.mapPixelToCoords(sf::Mouse::getPosition(window));
		}
		const sf::Vector2i MouseToGrid()
		{
			sf::Vector2f mousToView = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			return sf::Vector2i(
				int(mousToView.x / gridScale.x),
				int(mousToView.y / gridScale.y)
			);
		}
		const sf::UIntRect GridIndexsToCamera()
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
		}
		const void SetGameState(trait::GameState* _state) { this->pState = _state; }

		void SetResizeDistortion(const bool X_axis, const bool Y_axis)
		{
			this->distort_x = X_axis;
			if (!this->distort_x && this->bar_x)
				this->bar_x = false;

			this->distort_y = Y_axis;
			if (!this->distort_y && this->bar_y)
				this->bar_y = false;
		}
		void SetResizeBar(const bool X_axis, const bool Y_axis)
		{
			this->bar_x = X_axis;
			if (this->bar_x && !this->distort_x)
				this->distort_x = true;

			this->bar_y = Y_axis;
			if (this->bar_y && !this->distort_y)
				this->distort_y = true;
		}

		void SetGridScale(const sf::Vector2f _scale) { gridScale = _scale; }

		const sf::Font* GetFont() { return &font; };

		void UpdateObs(const trait::Subject* alteredSub);

		void UpdateCamera();
		void WindowResize();

		const std::list<Entity*> GetCameraEntities(const std::list<Entity*>& entities);
		sf::Texture* LoadTexture(std::string texturePath, sf::IntRect sheetCut = sf::IntRect(0, 0, 0, 0), bool repeated = false);

		void Draw(const sf::RectangleShape& drawTarget);
		void Draw(const sf::CircleShape& drawTarget);
		void Draw(const sf::Sprite& drawTarget);
		void Draw(const sf::Text& drawTarget);

		void Update();

	private:
		GraphicManager();
		~GraphicManager();

	private:
		static GraphicManager* instance;

		manager::EventManager::DebugFlagSubject* pDebugSubjectFlag;
		const sf::Vector2f originalSize;
		trait::GameState* pState;
		TextureMap textures;

		sf::RenderWindow window;
		sf::FloatRect cameraLim;
		sf::Vector2f gridScale;
		sf::Font font;
		sf::View view;

		float zoom;
		bool distort_x, distort_y;
		bool bar_x, bar_y;
		bool flagState;
	};
}