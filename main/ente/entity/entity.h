#pragma once

#include "../ente.h"
#include "manager/eventManager/eventManager.h"

namespace stage
{
	class Stage;
}

/*
	Classe de especialização de 'Ente' onde é um objeto que interage com outos 'dentro' do jogo.
*/
class Entity : public Ente, public trait::Observer
{
public:
	enum HitboxType : short int
	{
		BODY = 0,
		ATTACK_BOX
	};
	struct Hitbox
	{
		Hitbox() :
			type(HitboxType::BODY),
			normals(0, 0),
			pos(0.f, 0.f),
			size(0.f, 0.f),
			delta(0.f)
		{}
		Hitbox(const sf::Vector2f& _pos, const sf::Vector2f& _size,  const HitboxType _type = HitboxType::BODY,
			const sf::Vector2<short>& _normals = sf::Vector2<short>(0, 0), const float& _delta = 0.f) :
			type(_type),
			normals(_normals),
			pos(_pos),
			size(_size),
			delta(_delta)
		{}
		~Hitbox()
		{}

		HitboxType type = HitboxType::BODY;

		sf::Vector2<short> normals = sf::Vector2<short>(0, 0);
		sf::Vector2f pos = sf::Vector2f(0.f, 0.f);
		sf::Vector2f size = sf::Vector2f(0.f, 0.f);
		float delta = 0.f;
	};

public:
	Entity();
	Entity(const Type _type, const PrintPriority _priority, const sf::Vector2f _size,
		   const sf::Vector2f _position, const std::string _texturePath, const sf::IntRect sheetCut,
		   const bool _isStatic = true, const float _scale = 1.0f);
	virtual ~Entity();

	static void SetStage(stage::Stage* _stage) { pStage = _stage; }
	static void SetGravity(float* _gravity) { gravity = _gravity; }
	static void SetMaxGravity(float* _maxGravity) { maxGravity = _maxGravity; }

	const sf::Vector2f GetPosition() const
	{
		return this->hitBox.getPosition();
	}
	void SetPosition(const sf::Vector2f& _pos)
	{
		this->hitBox.setPosition(_pos);
		this->body.setPosition(_pos);
	}
	void MovePosition(const sf::Vector2f& _delta)
	{
		this->hitBox.move(_delta);
		this->body.move(_delta);
	}

	const float GetScale() const { return this->hitBox.getScale().x; }
	void SetScale(const float _scale)
	{
		this->hitBox.setScale(_scale, _scale);
		this->body.setScale(_scale, _scale);
	}

	const sf::Vector2f GetSize() const { return (this->hitBox.getSize() * this->hitBox.getScale().x); }
	const sf::FloatRect GetBounds() const
	{
		sf::Vector2f pos(this->hitBox.getPosition());
		sf::Vector2f size(this->GetSize() / 2.f);

		sf::FloatRect bounds(
			pos.x - size.x,
			pos.y - size.y,
			pos.x + size.x,
			pos.y + size.y
		);

		return bounds;
	}

	const sf::Vector2f& GetVelocity() const { return this->velocity; }
	void SetVelocityCoeff(const float _coeff) { this->velocityCoeff = _coeff; }
	void SetTexture(const std::string _texturePath, const sf::IntRect sheetCut = sf::IntRect(0, 0, 0, 0));

	virtual void UpdateObs(const trait::Subject* alteredSub);
	virtual const std::list<Hitbox> GetHitBoxes();

	virtual void Execute();
	virtual void DebugExecute();

	virtual void SelfPrint() = 0;
	virtual void DebugSelfPrint() = 0;

	virtual void EntityColision(Entity* _other, const sf::Vector2f& intersection) = 0;
	virtual void MapColision(const sf::Vector2f& intersection) = 0;

	virtual bool operator> (const Entity& other)
	{
		return this->type > other.type;
	};
	virtual bool operator< (const Entity& other)
	{
		return this->type < other.type;
	};
	virtual bool operator!= (const Entity& other)
	{
		return this->id != other.id;
	};
	virtual bool operator== (const Entity& other)
	{
		return this->id == other.id;
	};

protected:
	void SetHitBoxSize(const sf::Vector2f _size) { this->hitBox.setSize(_size);	}

	virtual void Move();

protected:
	static manager::EventManager::DebugFlagSubject* pDebugFlagSub;
	static stage::Stage* pStage;
	static float* maxGravity;
	static float* gravity;

	const bool isStatic;
	float velocityCoeff;

	sf::RectangleShape hitBox;
	sf::Texture* texture;
	sf::Sprite body;

	sf::Vector2f velocity;

	sf::CircleShape originCircle;
};