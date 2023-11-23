#include "initialStage.h"
using namespace stage;
using namespace character;
using namespace character::enemy;
using namespace obstacle;

InitialStage::InitialStage():
	Stage(
		sf::Vector2u(48, 13), 
		{
			"resources/textures/parallaxes/Background_0.png",
			"resources/textures/parallaxes/Background_1.png",
			"resources/textures/parallaxes/Background_2.png"
		}, 
		1.f)
{
	this->Initialize();
	this->DebugVerifyGridOcupation();
	this->background.ResetBackground();
};
InitialStage::~InitialStage()
{};

void InitialStage::Initialize()
{
	Player* p = new Player();
	Skeleton* e = new Skeleton();
	Demon* d = new Demon();

	p->SetPosition(sf::Vector2f(
		(0 * this->gridScale.x) + (p->GetSize().x / 2.f),
		(13 * this->gridScale.y) - (p->GetSize().y / 2.f)
	));
	e->SetPosition(sf::Vector2f(
		(16 * this->gridScale.x) + (p->GetSize().x / 2.f),
		(13 * this->gridScale.y) - (e->GetSize().y / 2.f)
	));
	d->SetPosition(sf::Vector2f(
		(16 * this->gridScale.x) + (p->GetSize().x / 2.f),
		(12 * this->gridScale.y) - (e->GetSize().y / 2.f)
	));

	this->EntityCreated(p);
	//this->EntityCreated(e);
	this->EntityCreated(d);
};