#pragma once

#include "manager/colisionManager/colisionManager.h"
#include "GUI/parallax/parallax.h"

namespace stage
{
	class Stage : public Ente, public trait::Observer
	{
	public:
		typedef std::pair<sf::RectangleShape, std::pair<sf::Vector2i, sf::Vector2i>> MouseTrack;
		typedef std::vector<std::vector<std::pair<sf::RectangleShape, sf::Text>>> GridBlock;
		typedef std::vector<auxiliary::NodeVec> Grid;

	public:
		Stage();
		Stage(const Stage& other) = delete;
		Stage(const sf::Vector2u gridSize, const std::vector<std::string> backgroundPaths, const float scale = 1.f);
		virtual ~Stage();

		const sf::Vector2f GetWorldSize() 
		{ 
			sf::Vector2f size(bounds.getSize());

			size.x *= bounds.getScale().x;
			size.y *= bounds.getScale().y;

			return size; 
		};
		const sf::Vector2f GetWorldPosition() { return this->bounds.getPosition(); };

		static const sf::Vector2f& GetGridScale() { return gridScale; };
		static Grid& GetGridReference() { return grid; };
		static GridBlock& GetGridBlockReference() { return gridBlock; };
		static const sf::Vector2i PositionToGrid(const sf::Vector2f pos)
		{
			return static_cast<sf::Vector2i>(sf::Vector2f(
					pos.x / gridScale.x,
					pos.y / gridScale.y
				)
			);
		};

		void Despawn(const unsigned long long int& id) { this->RemoveEntity(id); }

		virtual void UpdateObs(const trait::Subject* alteredSub);

		virtual void Execute();
		virtual void DebugExecute();
		virtual void SelfPrint();
		virtual void DebugSelfPrint();

	protected:
		virtual void Initialize() = 0;

		void OrganizeObstacles();
		void AddPlayer();
		void RemovePlayer();

		void VerifyGrid();
		void VerifyGridOcupation();
		void DebugVerifyGridOcupation();

		void EntityCreated(Entity* pEntity);
		void RemoveEntity(unsigned long long int id); 
		void RemoveEntity(const std::list<unsigned long long int>& ids);

	protected:
		static manager::EventManager::InputSubject* pInputSub;
		static sf::Vector2f gridScale;
		static GridBlock gridBlock;
		static Grid grid;

		static float gravity;
		static float maxGravityPull;

		bool showGrid;
		bool mousePressed;

		MouseTrack track;
		manager::ColisionManager* pColisionManager;
		GUI::Parallax background;

		EntityList entities;
		std::list<character::enemy::Enemy*> enemies;

		sf::RectangleShape bounds;
	};
}