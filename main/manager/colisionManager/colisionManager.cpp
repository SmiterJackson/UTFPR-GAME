#include "colisionManager.h"
#include "manager/graphicManager/graphicManager.h"
#include "stage/stage.h"
using namespace stage;
using namespace manager;
using namespace obstacle;
using namespace character;
using namespace character::enemy;

#define COLISION_CHECK_AMOUNT 100.f
#define CAST(x) static_cast<Entity*>(x)

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
	pGraphicManager(GraphicManager::GetInstance()),
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
}
void ColisionManager::AddRange(EntityList* _entities)
{
	unsigned int i = 0;

	if (_entities == nullptr)
		return;

	for (i = 0; i < _entities->Size(); i++)
		this->Add((*_entities)[i]);
}
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
}
void ColisionManager::RemoveRange(const std::list<unsigned long long int>& entitiesIds)
{
	std::list<unsigned long long int>::const_iterator idIt;
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
}

void ColisionManager::UpdateColisions(const float& elapsedTime)
{
	std::list<Entity*> entityList, obstaclesInCamera, enemiesInCamera;

	this->accumulator += elapsedTime;
	if (this->accumulator >= (1.f / COLISION_CHECK_AMOUNT))
	{
		this->accumulator -= (1.f / COLISION_CHECK_AMOUNT);

		for (Obstacle* o : this->obstacles)
			entityList.push_back(CAST(o));

		obstaclesInCamera = pGraphicManager->GetCameraEntities(entityList);
		entityList.clear();

		for (Enemy* e : this->enemies)
			entityList.push_back(CAST(e));

		enemiesInCamera = pGraphicManager->GetCameraEntities(entityList);

		for (Player* p : this->players)
		{
			for (Entity* e : obstaclesInCamera)
				this->CheckEntityColision(CAST(p), e, elapsedTime);

			for (Entity* e : enemiesInCamera)
			{
				for (Entity* o : obstaclesInCamera)
					this->CheckEntityColision(e, o, elapsedTime);

				this->CheckEntityColision(CAST(p), e, elapsedTime);
				this->CheckEntityColision(e, CAST(p), elapsedTime);

				CheckMapColision(e);
			}

			CheckMapColision(CAST(p));
			CheckCameraColision(p);
		}
	}
}

void ColisionManager::CheckEntityColision(Entity* entity, Entity* other, float elapsed)
{
	sf::Vector2f vel(entity->GetVelocity() * elapsed);

	if (vel.x == 0 && vel.y == 0)
		return;

	std::list<Entity::Hitbox> hitboxes = entity->GetHitBoxes();
	sf::Vector2f pos(other->GetPosition());
	sf::Vector2f origin;

	for (Entity::Hitbox hitbox : hitboxes)
	{
		origin = hitbox.pos;
		hitbox.size += other->GetSize();
		hitbox.pos = pos - hitbox.size / 2.f;

		if (RayCast(origin, vel, hitbox) && hitbox.delta <= 1.f)
			entity->EntityColision(other, hitbox.delta * hitbox.normals * sf::abs(vel));
	}
}
void ColisionManager::CheckCameraColision(Player* player)
{
	sf::Vector2f dist(player->GetPosition() - pGraphicManager->GetViewPosition());
	sf::Vector2f radious((pGraphicManager->GetViewSize() - player->GetSize()) / 2.f);
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
}
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
}

bool ColisionManager::RectsOverlaping(const sf::FloatRect& left, const sf::FloatRect& right)
{
	return (left.left < right.width && left.width > right.left &&
		left.top < right.height && left.height > right.top);
}
bool ColisionManager::RayCastTarget(Entity* entity, Entity* other, float elapsed, sf::Vector2f velocity)
{
	ColisionManager* colMan = GetInstance();

	if (velocity.x == 0 && velocity.y == 0 || colMan == nullptr)
		return false;

	std::list<Entity::Hitbox> entityHitboxes = entity->GetHitBoxes();
	std::list<Entity::Hitbox> hitboxes;
	sf::Vector2f origin;
	sf::Vector2f pos;

	for (Obstacle*& obs : colMan->obstacles)
		for (Entity::Hitbox hitbox : entityHitboxes)
		{
			pos = obs->GetPosition();

			origin = hitbox.pos;
			hitbox.size += obs->GetSize();
			hitbox.pos = pos - hitbox.size / 2.f;

			if (GetInstance()->RayCast(origin, velocity, hitbox))
				hitboxes.push_back(hitbox);
		}
	hitboxes.sort(
		[&](const Entity::Hitbox& l, const Entity::Hitbox& r)
		{
			return l.delta < r.delta;
		}
	);

	pos = other->GetPosition();
	for (Entity::Hitbox hitbox : entityHitboxes)
	{
		origin = hitbox.pos;
		hitbox.size += other->GetSize();
		hitbox.pos = pos - hitbox.size / 2.f;

		// Verifica se a entidade colide com outra antes de qualquer obstáculo, tendo um caminho livre
		if (GetInstance()->RayCast(origin, velocity, hitbox))
		{
			if(hitboxes.size() > 0 && hitbox.delta > hitboxes.front().delta)
				return false;
			return true;
		}
	}

	return false;
}
bool ColisionManager::ValidSpace(const sf::FloatRect& bounds)
{
	std::list<Obstacle*> obstacles = ColisionManager::GetInstance()->obstacles;
	sf::FloatRect obsBounds;
	bool valid = true;

	obstacles.remove_if(
		[&](const Obstacle* obs)
		{
			obsBounds = obs->GetBounds();
			return (obsBounds.width < bounds.left ||
					obsBounds.left > bounds.width ||
					obsBounds.height < bounds.top ||
					obsBounds.top > bounds.height );
		}
	);

	for(Obstacle* obs : obstacles)
	{
		obsBounds = obs->GetBounds();

		valid = !RectsOverlaping(bounds, obsBounds);

		if (!valid)
			break;
	}

	return valid;
}
bool ColisionManager::RayCast(const sf::Vector2f& origin, const sf::Vector2f& dir, Entity::Hitbox& trg)
{
	sf::Vector2f t_near((trg.pos - origin) / dir);
	sf::Vector2f t_far((trg.pos + trg.size - origin) / dir);
	float delta_far = 0.f;

	if (std::isnan(t_far.x) || std::isnan(t_far.y) || std::isnan(t_near.x) || std::isnan(t_near.y))
		return false;

	if (t_near.x > t_far.x) 
		std::swap(t_near.x, t_far.x);
	if (t_near.y > t_far.y) 
		std::swap(t_near.y, t_far.y);

	if (t_near.x > t_far.y || t_near.y > t_far.x)
		return false;

	trg.delta = std::max(t_near.x, t_near.y);
	delta_far = std::min(t_far.x, t_far.y);

	if (delta_far < 0)
		return false;

	trg.pos = origin + t_near * dir;
	trg.normals = sf::Vector2<short>(
		short((t_near.x > t_near.y) * (1 - (2 * (dir.x < 0)))),
		short((t_near.x < t_near.y) * (1 - (2 * (dir.y < 0))))
	);

	return true;
};