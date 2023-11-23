#include "enemy.h"
#include "stage/stage.h"
using namespace character::enemy;
using namespace auxiliary;
using namespace manager;
using namespace stage;

#define CAST(x) static_cast<unsigned int>(x)

EventManager::InputSubject*		Enemy::pInputSub	= EventManager::InputSubject::GetInstance();
stage::Stage::Grid&				Enemy::grid			= Stage::GetGridReference();
Enemy::GridBlock&				Enemy::gridBlock	= Stage::GetGridBlockReference();
const PlayerList&				Enemy::players		= Player::GetPlayerList();
bool							Enemy::showAStar	= false;

Enemy::Enemy() :
	Character(
		Type::ENEMY, sf::Vector2f(0.5f, 0.5f), sf::Vector2f(0.f, 0.f), ""
	),
	target(nullptr),
	agroRange(0.f),
	agroing(false)
{
	pInputSub->AttachObs(this);
};
Enemy::Enemy(const sf::Vector2f _size, const sf::Vector2f _tokenSize, const std::string _texturePath, 
		const AnimationSheet _animations, const short int _lifeAmount, const float _invcDuration,
		const float _agroRange,	const bool _isStatic, const float _scale) :
	Character(
		Type::ENEMY, _size, _tokenSize, _texturePath, _animations,
		_lifeAmount, _invcDuration, _isStatic, _scale
	),
	target(nullptr),
	agroRange(_agroRange),
	agroing(false)
{
	pInputSub->AttachObs(this);
};
Enemy::~Enemy() 
{
	pInputSub->DettachObs(this);
};

void Enemy::UpdateTarget()
{
	sf::Vector2f srcPos(this->GetPosition());
	sf::Vector2f trgPos;
	double delta(0.0), least = INFINITY;

	for (Player* p : players)
	{
		trgPos = p->GetPosition();
		delta = sqrt(pow(trgPos.x - srcPos.x, 2) + pow(trgPos.y - srcPos.y, 2));

		if (delta <= least)
		{
			least = delta;
			this->target = p;
		}
	}
};

Enemy::DIRECTION Enemy::AStarAlgorithm(const sf::Vector2i srcGridPos, const sf::Vector2i trgGridPos)
{
	if (CAST(srcGridPos.x) >= grid.size() || CAST(srcGridPos.y) >= grid[0].size() ||
		CAST(trgGridPos.x) >= grid.size() || CAST(trgGridPos.y) >= grid[0].size())
		return DIRECTION::ERROR;

	unsigned int x = 0, y = 0;
	double delta = 0.0;
	Node* current = &grid[srcGridPos.x][srcGridPos.y];
	Node* target  = &grid[trgGridPos.x][trgGridPos.y];
	Node* startNode = current;
	std::list<Node*> notTested;
	sf::Vector2i start = current->position, next;

	if (current == nullptr || target == nullptr)
		return DIRECTION::ERROR;

	for(x = 0; x < grid.size(); x++)
		for (y = 0; y < grid[x].size(); y++)
		{
			grid[x][y].tested = false;
			grid[x][y].distance = INFINITY;
			grid[x][y].heuristic = INFINITY;
			grid[x][y].previous = nullptr;
		}

	current->distance = 0.0;
	current->heuristic = Node::CalcHeuristic(current, target);
	notTested.push_back(current);

	while(!notTested.empty() && current != target)
	{
		notTested.sort(
			[](const Node* lNode, const Node* rNode)
			{
				return lNode->heuristic < rNode->heuristic;
			}
		);

		while (!notTested.empty() && notTested.front()->tested)
			notTested.pop_front();

		if (notTested.empty())
			break;

		current = notTested.front();
		if (current == nullptr)
			continue;

		current->tested = true;

		for(Node* neighbour : current->neighbours)
		{
			if (neighbour == nullptr)
				continue;

			if (!neighbour->tested && !neighbour->obstacle)
				notTested.push_back(neighbour);

			delta = current->distance + Node::CalcDistance(current, neighbour);

			if(delta < neighbour->distance)
			{
				neighbour->previous = current;
				neighbour->distance = delta;
				neighbour->heuristic = neighbour->distance + Node::CalcHeuristic(neighbour, target);
			}
		}
	}
	if (current != target)
		return DIRECTION::ERROR;

	if (showAStar)
		tracePath(*current);

	while(current->previous != startNode && current->previous != nullptr)
	{
		current = current->previous;
	}
	next = current->position;

	return DIRECTION(
		((next.x <  start.x && next.y <  start.y) * DIRECTION::UP_LEFT	) +
		((next.x == start.x && next.y <  start.y) * DIRECTION::UP		) +
		((next.x >  start.x && next.y <  start.y) * DIRECTION::UP_RIGHT	) +
		((next.x <  start.x && next.y == start.y) * DIRECTION::LEFT		) +
		((next.x >  start.x && next.y == start.y) * DIRECTION::RIGHT	) +
		((next.x <  start.x && next.y >  start.y) * DIRECTION::DOWN_LEFT) +
		((next.x == start.x && next.y >  start.y) * DIRECTION::DOWN		) +
		((next.x >  start.x && next.y >  start.y) * DIRECTION::DOWN_RIGHT)
	);
};
void Enemy::tracePath(Node& trg)
{
	unsigned int i = 0, j = 0;
	Node* current = &trg;

	std::stack<sf::Vector2i> Path;

	for (i = 0; i < grid.size(); i++)
		for (j = 0; j < grid[i].size(); j++)
			if (!grid[i][j].obstacle)
				gridBlock[i][j].first.setFillColor(sf::Color(0U, 0U, 255U, 100U));

	while (current->previous != nullptr)
	{
		Path.push(current->position);
		current = current->previous;
	}
	Path.push(current->position);

	while (!Path.empty())
	{
		sf::Vector2i p = Path.top();
		gridBlock[p.x][p.y].first.setFillColor(sf::Color(255U, 255U, 0U, 100U));
		Path.pop();
	}

	return;
};