#pragma once

#include "stdafx.h"

namespace auxiliary
{
	struct Node
	{
		Node(const bool _obstacle = false, sf::Vector2i _position = sf::Vector2i(0, 0));
		~Node();

		static double CalcHeuristic(const Node* src, const Node* trg); 
		static double CalcDistance(const Node* src, const Node* trg);

		bool obstacle = false;
		bool tested   = false;

		double distance = 0.0;
		double heuristic = 0.0;

		sf::Vector2i position = sf::Vector2i(0, 0);

		Node* previous = nullptr;
		std::vector<Node*> neighbours = std::vector<Node*>();
	};

	typedef std::vector<Node> NodeVec;
	typedef std::list<Node> NodeList;
}