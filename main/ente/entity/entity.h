#pragma once

#include "../ente.h"
#include "manager/eventManager/eventManager.h"

/*
	Classe de especializa��o de 'Ente' onde � um objeto que interage com outos 'dentro' do jogo.
*/
class Entity : public Ente, public trait::Observer
{
public:
	Entity();
	Entity(const unsigned short int _type, const unsigned short int _printPriority, const sf::Vector2f _size,
		const sf::Vector2f _position, const std::string _texturePath, const sf::IntRect sheetCut,
		const bool _isStatic = true, const float _scale = 1.0f);
	virtual ~Entity();

	const unsigned long long int GetId() const { return this->id; };

	void SetVelocityCoeff(const float _coeff) { this->velocityCoeff = _coeff; };
	const bool IsStatic() const { return this->isStatic; }

	void SetTexture(const std::string _texturePath, const sf::IntRect sheetCut = sf::IntRect(0, 0, 0, 0));

	void SetScale(const float _scale)
	{
		this->hitBox.setScale(_scale, _scale);
		this->body.setScale(_scale, _scale);
	}
	const float GetScale() const { return this->hitBox.getScale().x; }
	sf::Vector2f& GetVelocity() { return this->velocity; };

	virtual void SetHitBoxSize(const sf::Vector2f _size) 
	{ 
		this->hitBox.setSize(_size); 
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

	const sf::Vector2f GetPosition() const;
	virtual void MovePosition(const float X_axis, const float Y_axis);
	virtual void SetPosition(const float X_axis, const float Y_axis);

	virtual void UpdateObs(const trait::Subject* alteredSub);

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

protected:
	virtual void Move();

protected:
	static manager::EventManager::DebugFlagSubject* pDebugFlagSub;

	float velocityCoeff;
	const bool isStatic;

	sf::RectangleShape hitBox;
	sf::Texture* texture;
	sf::Sprite body;

	sf::Vector2f velocity;

	sf::CircleShape originCircle;
};