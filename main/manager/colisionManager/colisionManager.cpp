#include "colisionManager.h"
#include "manager/graphicManager/graphicManager.h"
#include "stage/stage.h"
using namespace stage;
using namespace manager;
using namespace obstacle;
using namespace character;
using namespace character::enemy;

#define COLISION_CHECK_TIMER 0.01f
#define CAST(x) static_cast<Entity*>(x)
#define SF_ABS(vec) sf::Vector2f(std::abs(vec.x), std::abs(vec.y))

stage::Stage* ColisionManager::pOwner = nullptr;
ColisionManager* ColisionManager::instance = nullptr;

ColisionManager* ColisionManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new ColisionManager();
		if (instance == nullptr)
			std::cerr << "Nao foi possivel instanciar um gerenciador de colisões: colision_manager." << std::endl;
	}

	return instance;
};
void ColisionManager::DeconstructInstance()
{
	if (instance != nullptr)
		delete instance;
};

ColisionManager::ColisionManager() :
	players(Player::GetPlayerList()),
	obstacles(),
	enemies(),
	accumulator(0.f)
{};
ColisionManager::~ColisionManager()
{};

void ColisionManager::Add(Entity* entity)
{
	switch (entity->GetType())
	{
	case Type::ENEMY:
		this->enemies.push_back(static_cast<Enemy*>(entity));
		break;
	case Type::OBSTACLE:
		this->obstacles.push_back(static_cast<Obstacle*>(entity));
		break;
	default:
		break;
	}
};
void ColisionManager::AddRange(EntityList* _entities)
{
	unsigned int i = 0;

	if (_entities == nullptr)
		return;

	for (i = 0; i < _entities->GetSize(); i++)
		this->Add((*_entities)[i]);
};
void ColisionManager::Remove(const unsigned long long int entityId)
{
	std::list<Obstacle*>::const_iterator oIt;
	std::vector<Enemy*>::const_iterator eIt;

	for (eIt = this->enemies.cbegin(); eIt != this->enemies.cend(); eIt++)
		if ((*eIt)->GetId() == entityId)
		{
			this->enemies.erase(eIt);
			return;
		}

	for (oIt = this->obstacles.cbegin(); oIt != this->obstacles.cend(); oIt++)
		if ((*oIt)->GetId() == entityId)
		{
			this->obstacles.erase(oIt);
			return;
		}
};
void ColisionManager::RemoveRange(const std::vector<unsigned long long int>& entitiesIds)
{
	std::vector<unsigned long long int>::const_iterator idIt;
	std::list<Obstacle*>::const_iterator oIt;
	std::vector<Enemy*>::const_iterator eIt;

	for (idIt = entitiesIds.cbegin(); idIt != entitiesIds.cend(); idIt++)
	{
		for (eIt = this->enemies.cbegin(); eIt != this->enemies.cend(); eIt++)
			if ((*eIt)->GetId() == (*idIt))
			{
				this->enemies.erase(eIt);
				goto END;
			}

		for (oIt = this->obstacles.cbegin(); oIt != this->obstacles.cend(); oIt++)
			if ((*oIt)->GetId() == (*idIt))
			{
				this->obstacles.erase(oIt);
				break;
			}

	END:;
	}
};

void ColisionManager::UpdateColisions(const float& elapsedTime)
{
	std::list<Obstacle*> obstaclesInCamera;
	std::list<Enemy*> enemiesInCamera;

	this->accumulator += elapsedTime;
	if (this->accumulator >= COLISION_CHECK_TIMER)
	{
		this->accumulator -= COLISION_CHECK_TIMER;

		obstaclesInCamera = GraphicManager::GetCameraEntities(this->obstacles);
		enemiesInCamera = GraphicManager::GetCameraEntities(this->enemies);

		for (Player* p : this->players)
		{
			CheckMapColision(CAST(p));
			CheckCameraColision(p);

			for (Obstacle* o : obstaclesInCamera)
				this->CheckEntityColision(CAST(p), CAST(o), elapsedTime);

			for (Enemy* e : enemiesInCamera)
			{
				CheckMapColision(CAST(e));

				for (Obstacle* o : obstaclesInCamera)
					this->CheckEntityColision(CAST(e), CAST(o), elapsedTime);

				this->CheckEntityColision(CAST(p), CAST(e), elapsedTime);
				this->CheckEntityColision(CAST(e), CAST(p), elapsedTime);
			}
		}
	}
};

void ColisionManager::CheckEntityColision(Entity* entity, Entity* other, float elapsed)
{
	sf::Vector2f vel(entity->GetVelocity());

	if (vel.x == 0 && vel.y == 0)
		return;

	sf::Vector2f pos(other->GetPosition());
	sf::Vector2f size(other->GetSize());
	sf::Vector2f entSize(entity->GetSize());
	sf::Vector2<short int> normals(0, 0);

	float delta = 0.f;
	bool intersected = false;

	HitBox hitbox(
		pos - (entSize / 2.f) - (size / 2.f),
		size + entSize
	);

	pos = entity->GetPosition();
	intersected = VectToHitBox(pos, vel * elapsed, hitbox, normals, delta);

	if (intersected && delta <= 1.f)
		entity->EntityColision(other, delta * normals * elapsed * SF_ABS(vel));
};
void ColisionManager::CheckMapColision(Entity* entity)
{
	sf::Vector2f dist(entity->GetPosition() - this->pOwner->GetWorldPosition());
	sf::Vector2f radious((this->pOwner->GetWorldSize() - entity->GetSize()) / 2.f);
	sf::Vector2f intersection(
		fabs(dist.x) - radious.x,
		fabs(dist.y) - radious.y
	);

	if (intersection.x > 0.f || intersection.y > 0.f)
	{
		/*
		if (intersection.x <= 0.f)
			intersection.x = 0.f;

		if (intersection.y <= 0.f)
			intersection.y = 0.f;

		if (dist.x > 0.f)
			intersection.x = -(intersection.x);

		if (dist.y > 0.f)
			intersection.y = -(intersection.y);
		*/
		// versao branchless
		intersection = sf::Vector2f(
			(intersection.x * ((1.f - (2.f * (dist.x > 0.f))) * (intersection.x > 0.f))),
			(intersection.y * ((1.f - (2.f * (dist.y > 0.f))) * (intersection.y > 0.f)))
		);

		entity->MapColision(intersection);
	}
};
void ColisionManager::CheckCameraColision(Player* player)
{
	sf::Vector2f dist(player->GetPosition() - GraphicManager::GetViewPosition());
	sf::Vector2f radious((GraphicManager::GetViewSize() - player->GetSize()) / 2.f);
	sf::Vector2f intersection(
		fabs(dist.x) - radious.x,
		fabs(dist.y) - radious.y
	);

	if (intersection.x > 0.f || intersection.y > 0.f)
	{
		/*
		if (intersection.x <= 0.f)
			intersection.x = 0.f;

		if (intersection.y <= 0.f)
			intersection.y = 0.f;

		if (dist.x > 0.f)
			intersection.x = -(intersection.x);

		if (dist.y > 0.f)
			intersection.y = -(intersection.y);
		*/
		// versao branchless
		intersection = sf::Vector2f(
			(intersection.x * ((1.f - (2.f * (dist.x > 0.f))) * (intersection.x > 0.f))),
			(intersection.y * ((1.f - (2.f * (dist.y > 0.f))) * (intersection.y > 0.f)))
		);
		player->CameraColision(intersection);
	}
};

bool ColisionManager::VectToHitBox(const sf::Vector2f& origin, const sf::Vector2f& dir, const HitBox& target,
								   sf::Vector2<short>& normal, float& delta)
{
	sf::Vector2f invdir(1.0f / dir);
	sf::Vector2f t_near((target.pos - origin) * invdir);
	sf::Vector2f t_far((target.pos + target.size - origin) * invdir);
	float far = 0.f;

	if (std::isnan(t_far.x) || std::isnan(t_far.y) || std::isnan(t_near.x) || std::isnan(t_near.y))
		return false;

	if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
	if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

	if (t_near.x > t_far.y || t_near.y > t_far.x)
		return false;

	delta = std::max(t_near.x, t_near.y);
	far = std::min(t_far.x, t_far.y);

	if (far < 0)
		return false;

	normal = sf::Vector2<short>(
		(t_near.x > t_near.y) * (1 - (2 * (invdir.x < 0))),
		(t_near.x < t_near.y) * (1 - (2 * (invdir.y < 0)))
	);

	return true;
};