#include "parallax.h"
#include "manager/graphicManager/graphicManager.h"
using namespace GUI;
using namespace manager;

#define NUM_BCKG_INSTANCES 3U

Parallax::Parallax(const std::vector<std::string>& paths, const float _scale):
	backGrounds(),
	lastPos(sf::Vector2f(0.f, 0.f)),
	layers_spd(1.f),
	scale(_scale)
{
	this->SetBackgrounds(paths);
};
Parallax::~Parallax()
{};

void Parallax::SetBackgrounds(const std::vector<std::string>& paths)
{
	std::vector<std::string>::const_iterator cIt;
	std::vector<sf::Sprite>* pLastSpriteVec = nullptr;
	sf::Sprite* pLastSprite = nullptr;
	sf::Texture* txt = nullptr;
	size_t i = 0;

	if (this->backGrounds.size() > 0)
		this->backGrounds.clear();

	/*
		Para cada path de textura para efeito parallax cria 2 objetos de 'Background' subsequentes em ordem na
		lista e em posição na tela, tal que, cada qual com o tamanho equilavente à 2 vezes a própria textura,
		efetivando o efeito de 4 texturas iguais em fila plotados na tela
	*/
	this->backGrounds.reserve(paths.size());
	for (cIt = paths.cbegin(); cIt != paths.cend(); cIt++)
	{
		txt = GraphicManager::LoadTexture(*cIt);
		this->backGrounds.emplace_back(Layer(txt, std::vector<sf::Sprite>()));
		pLastSpriteVec = &this->backGrounds.back().second;

		pLastSpriteVec->reserve(NUM_BCKG_INSTANCES);
		for (i = 0; i < NUM_BCKG_INSTANCES; i++)
		{
			pLastSpriteVec->emplace_back(sf::Sprite());
			pLastSprite = &pLastSpriteVec->back();

			pLastSprite->setTexture(*txt);
			pLastSprite->setPosition(0.f, 0.f);
			pLastSprite->setScale(this->scale, this->scale);
			pLastSprite->move((txt->getSize().x * this->scale * i), 0.f);
		}
	}

	this->layers_spd = 1.f / this->backGrounds.size();
	this->lastPos = GraphicManager::GetViewPosition();
};
void Parallax::SetSizeCoefficient(float _scale)
{
	std::vector<Layer>::iterator it;
	size_t i = 0;
	this->scale = _scale;

	for (it = this->backGrounds.begin(); it != this->backGrounds.end(); it++)
		for (i = 0; i < it->second.size(); i++)
			it->second[i].setScale(_scale, _scale);
};

void Parallax::ResetBackground()
{
	unsigned int i = 0;

	for (Layer layer : this->backGrounds)
	{
		i = 0;
		for (sf::Sprite sprite : layer.second)
		{
			sprite.setPosition(0.f, 0.f);
			sprite.move((layer.first->getSize().x * this->scale * i), 0.f);
			i++;
		}
	}

	this->lastPos = GraphicManager::GetViewPosition();
};
void Parallax::SelfPrint()
{
	std::vector<Layer>::reverse_iterator rIt;
	size_t i = 0;

	for (rIt = this->backGrounds.rbegin(); rIt != this->backGrounds.rend(); rIt++)
		for (i = 0; i < rIt->second.size(); i++)
			GraphicManager::Draw(rIt->second[i]);
};
void Parallax::Execute()
{
	std::vector<Layer>::iterator it;
	sf::Sprite* pSprite = nullptr;
	sf::Vector2f diff, movement;
	sf::Vector2f viewPos(GraphicManager::GetViewPosition());
	sf::Vector2f viewSize(GraphicManager::GetViewSize());
	float leastLim = 0.f, maxLim = 0.f, counter = 1.f;
	unsigned int i = 0;

	diff = this->lastPos - viewPos;
	leastLim	= viewPos.x - viewSize.x;
	maxLim		= viewPos.x + viewSize.x;
	this->lastPos = viewPos;

	/*
		Para a quantidade de "layers", camadas, de backgrounds em função do número de cópias/instancias,
		move cada camada proporcinalmente a sua respectiva velocidade dado pela "posição da camada"
	*/
	if (diff.x != 0.f || diff.y != 0.f)
	{
		for (it = this->backGrounds.begin(), counter = 0;
			it != this->backGrounds.end();
			counter++, it++)
		{
			movement = diff * (-layers_spd * counter);

			for (i = 0; i < it->second.size(); i++)
			{
				pSprite = &it->second[i];

				pSprite->move(movement);

				if (pSprite->getPosition().x + pSprite->getGlobalBounds().width < leastLim)
					pSprite->move((pSprite->getGlobalBounds().width * NUM_BCKG_INSTANCES), 0.f);

				else if (pSprite->getPosition().x > maxLim)
					pSprite->move(-(pSprite->getGlobalBounds().width * NUM_BCKG_INSTANCES), 0.f);
			}
		}
	}
};