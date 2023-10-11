#include "node.h"
using namespace auxiliary;

Node::Node(const bool _obstacle, sf::Vector2i _position)
{
	this->obstacle = _obstacle;
	this->position = _position;
};
Node::~Node()
{};

double Node::CalcHeuristic(const Node* src, const Node* trg)
{
	double bias(10.0 * (trg->position.y < src->position.y));
	double deltaX = double(abs(src->position.x - trg->position.x));
	double deltaY = double(abs(src->position.y - trg->position.y));

	return deltaY * bias + deltaX;
};
double Node::CalcDistance(const Node* src, const Node* trg)
{
	return (
		sqrt(
			pow(src->position.x - trg->position.x, 2) + pow(src->position.y - trg->position.y, 2)
		)
	);
};