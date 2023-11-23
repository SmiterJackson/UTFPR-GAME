#include "eventManager.h"
#include "../graphicManager/graphicManager.h"
using namespace manager;

EventManager*		EventManager::instance = nullptr;

EventManager::DebugFlagSubject* EventManager::DebugFlagSubject::instance = nullptr;
EventManager::InputSubject*		EventManager::InputSubject::instance = nullptr;



EventManager::DebugFlagSubject* EventManager::DebugFlagSubject::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new EventManager::DebugFlagSubject();
		if (instance == nullptr)
			std::cerr << "Não foi possível instanciar um DebugFlagSubject. class Game::DebugFlagSubject." << std::endl;
	}

	return instance;
};
void EventManager::DebugFlagSubject::DeconsInstance()
{
	if (instance != nullptr)
		delete instance;
};

EventManager::DebugFlagSubject::DebugFlagSubject() :
	debugFlag(false)
{};
EventManager::DebugFlagSubject::~DebugFlagSubject()
{};

void EventManager::DebugFlagSubject::SetDebugFlag(const bool flagState)
{
	this->debugFlag = flagState;
	this->NotifyAllObs();
};
const bool EventManager::DebugFlagSubject::GetDebugFlag()
{
	return this->debugFlag;
};



EventManager::InputSubject* EventManager::InputSubject::GetInstance()
{
	if(instance == nullptr)
	{
		instance = new EventManager::InputSubject();
		if(instance == nullptr)
			std::cerr << "Não foi possível alocar um InputSubject. class: EventManager::InputSubject." << std::endl;
	}

	return instance;
};
void EventManager::InputSubject::DeconsInstance()
{
	if (instance == nullptr)
		delete instance;
};

EventManager::InputSubject::InputSubject():
	_event()
{};
EventManager::InputSubject::~InputSubject()
{};

void EventManager::InputSubject::SetEvent(const sf::Event& _newEvent)
{
	this->_event = _newEvent;
	this->NotifyAllObs();
};
const sf::Event& EventManager::InputSubject::GetEvent()
{
	return this->_event;
};



EventManager* EventManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new EventManager();
		if (instance == nullptr)
			std::cerr << "Não foi possível alocar um EventManager. class: EventManager." << std::endl;
	}

	return instance;
};
void EventManager::DeconsInstance()
{
	if (instance == nullptr)
		delete instance;
};

EventManager::EventManager():
	pGraphicManager(GraphicManager::GetInstance()),
	pInputSubject(InputSubject::GetInstance()),
	pDebugFlagSub(DebugFlagSubject::GetInstance())
{
	this->pInputSubject->AttachObs(this);
};
EventManager::~EventManager()
{
	this->pInputSubject->DettachObs(this);
};

void EventManager::UpdateObs(const trait::Subject* alteredSub)
{
	if (alteredSub == nullptr || alteredSub != this->pInputSubject)
		return;

	sf::Event _event = pInputSubject->GetEvent();
	bool ctrl_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
	bool shift_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);

	if (_event.type == sf::Event::KeyPressed)
	{
		if (_event.key.code == sf::Keyboard::D && ctrl_pressed && shift_pressed)
		{
			this->pDebugFlagSub->SetDebugFlag(!this->pDebugFlagSub->GetDebugFlag());
		}
	}
};

void EventManager::Update()
{
	sf::Event _event;
	if (pGraphicManager == nullptr)
		return;

	while(pGraphicManager->GetWindow().pollEvent(_event))
	{
		switch (_event.type)
		{
		case sf::Event::Closed:
			pGraphicManager->CloseWindow();
			break;
		case sf::Event::Resized:
			pGraphicManager->WindowResize();
			break;
		default:
			this->pInputSubject->SetEvent(_event);
			break;
		}
	}
};