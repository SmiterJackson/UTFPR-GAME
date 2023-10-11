#include "stage.h"
#include "ente/entity/obstacle/tile/tile.h"
#include "manager/graphicManager/graphicManager.h"
using namespace auxiliary;
using namespace obstacle;
using namespace manager;
using namespace stage;
using namespace GUI;

#define GRID_SCALE sf::Vector2f(32.f, 32.f)
#define CAM_LIMIT_SCALE 1.0f

float						Stage::gravity			= 10.f;
float						Stage::maxGravityPull	= 6.5f;
EventManager::InputSubject* Stage::pInputSub		= EventManager::InputSubject::GetInstance();
sf::Vector2f				Stage::gridScale		= GRID_SCALE;
Stage::GridBlock			Stage::gridBlock		= Stage::GridBlock();
Stage::Grid					Stage::grid				= Stage::Grid();

Stage::Stage():
	Ente(),
	Observer(),
	showGrid(false),
	mousePressed(false),
	track(),
	pColisionManager(ColisionManager::GetInstance()),
	background(),
	entities(),
	enemies(),
	bounds()
{
	GraphicManager::SetCameraLimits(sf::FloatRect(0.f, 0.f, 0.f, 0.f));

	ColisionManager::SetOwner(this);

	GraphicManager::UpdateCamera();
	this->background.ResetBackground();

	if(pInputSub != nullptr)
		this->pInputSub->AttachObs(this);
};
Stage::Stage(const sf::Vector2u gridSize, const std::vector<std::string> backgroundPaths, const float scale) :
	Ente(Type::STAGE, PrintPriority::background),
	Observer(),
	showGrid(false),
	mousePressed(false),
	track(),
	pColisionManager(ColisionManager::GetInstance()),
	background(backgroundPaths, 1.f),
	entities(),
	enemies(),
	bounds()
{
	unsigned int x = 0U, y = 0U;
	unsigned int j = 0U, h = 0U;

	gridScale = GRID_SCALE;

	sf::Vector2f size(gridSize.x * gridScale.x, gridSize.y * gridScale.y);
	sf::RectangleShape baseShape;
	sf::Text txt;

	this->bounds.setSize(size);
	this->bounds.setOrigin(size / 2.f);
	this->bounds.setPosition(size / 2.f);
	this->bounds.setFillColor(sf::Color::Transparent);
	this->bounds.setOutlineThickness(1.5f);
	this->bounds.setOutlineColor(sf::Color::Red);

	txt.setFont(*GraphicManager::GetFont());
	txt.setCharacterSize(10);
	baseShape.setSize(gridScale);
	baseShape.setFillColor(sf::Color(0U, 0U, 255U, 100U));
	baseShape.setOutlineColor(sf::Color::Magenta);
	baseShape.setOutlineThickness(0.0f);

	this->track.first.setFillColor(sf::Color::Transparent);
	this->track.first.setOutlineColor(sf::Color::Yellow);
	this->track.first.setOutlineThickness(1.f);
	
	grid.resize(gridSize.x);
	gridBlock.resize(gridSize.x);
	for (x = 0U; x < gridSize.x; x++)
	{
		grid[x].resize(gridSize.y);
		gridBlock[x].resize(gridSize.y, std::make_pair(baseShape, txt));
	}

	if (grid.size() > gridSize.x)
	{
		std::cerr << "N�o foi poss�vel alocar o n�mero necess�rio de linhas para o Stage::grid." << std::endl;
		return;
	}
	for (x = 0U; x < gridSize.x; x++)
		if (grid[x].size() > gridSize.y)
		{
			std::cerr << "N�o foi poss�vel alocar o n�mero necess�rio de colunas para o Stage::grid." << std::endl;
			return;
		}

	for (x = 0U; x < gridSize.x; x++)
		for (y = 0U; y < gridSize.y; y++)
		{
			std::stringstream ss;
			ss << "(" << x << ", " << y << ")";

			gridBlock[x][y].second.setString(ss.str());
			gridBlock[x][y].second.setPosition(gridScale.x * x, gridScale.y * y);
			gridBlock[x][y].first.setPosition(gridScale.x * x, gridScale.y * y);
			grid[x][y].position = sf::Vector2i(x, y);

			for (j = x - (x > 0); j <= (x + (x < (gridSize.x - 1))); j++)
				for (h = y - (y > 0); h <= (y + (y < (gridSize.y - 1))); h++)
					if(x != j || y != h)
						grid[x][y].neighbours.push_back(&grid[j][h]);
		}

	GraphicManager::SetCameraLimits(sf::FloatRect(
		-size.x * (CAM_LIMIT_SCALE - 1.f),
		-size.y * (CAM_LIMIT_SCALE - 1.f),
		 size.x * CAM_LIMIT_SCALE,
		 size.y * CAM_LIMIT_SCALE)
	);
	GraphicManager::SetGridScale(gridScale);

	ColisionManager::SetOwner(this);

	GraphicManager::UpdateCamera();
	this->background.ResetBackground();
	if (pInputSub != nullptr)
		this->pInputSub->AttachObs(this);
};
Stage::~Stage()
{
	ColisionManager::DeconstructInstance();
};

void Stage::UpdateObs(const trait::Subject* alteredSub) 
{
	if (alteredSub == nullptr || alteredSub != pInputSub)
		return;

	sf::Event _event = pInputSub->GetEvent();

	int  i = 0, j = 0;
	auto lesser = [&](const sf::Vector2i& left, const sf::Vector2i& right) -> sf::Vector2i
	{
		return sf::Vector2i(
			(left.x * (left.x <= right.x)) + (right.x * (right.x < left.x)),
			(left.y * (left.y <= right.y)) + (right.y * (right.y < left.y))
		);
	};
	auto bigger = [&](const sf::Vector2i& left, const sf::Vector2i& right) -> sf::Vector2i
	{
		return sf::Vector2i(
			(left.x * (left.x >= right.x)) + (right.x * (right.x > left.x)),
			(left.y * (left.y >= right.y)) + (right.y * (right.y > left.y))
		);
	};

	bool ctrl_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
	bool shift_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);

	if (_event.type == sf::Event::EventType::KeyPressed)
	{
		if (_event.key.code == sf::Keyboard::G && ctrl_pressed && shift_pressed)
			showGrid = !showGrid;

		if (_event.key.code == sf::Keyboard::O && ctrl_pressed && shift_pressed)
			OrganizeObstacles();

		if (showGrid)
		{
			for (auto& line : gridBlock)
				for (auto& block : line)
					block.first.setOutlineThickness(0.4f);
		}
		else 
		{
			for (auto& line : gridBlock)
				for (auto& block : line)
					block.first.setOutlineThickness(0.0f);
		}
	}

	if (_event.type == sf::Event::MouseButtonPressed ||
		_event.type == sf::Event::MouseButtonReleased ||
		_event.type == sf::Event::MouseMoved)
	{
		if (_event.type == sf::Event::MouseButtonPressed && 
		   (_event.mouseButton.button == sf::Mouse::Left || _event.mouseButton.button == sf::Mouse::Right))
		{
			auto index = GraphicManager::MouseToGrid();
			track.second.first = index;
			track.second.second = index;
			track.first.setSize(gridScale);
			track.first.setPosition(
				index.x * gridScale.x,
				index.y * gridScale.y
			);

			if (_event.mouseButton.button == sf::Mouse::Left)
				grid[index.x][index.y].obstacle = true;
			else
				grid[index.x][index.y].obstacle = false;

			mousePressed = true;
		}
		else if (_event.type == sf::Event::MouseButtonReleased &&
				(_event.mouseButton.button == sf::Mouse::Left || _event.mouseButton.button == sf::Mouse::Right))
		{
			sf::Vector2i minor(lesser(track.second.first, track.second.second));
			sf::Vector2i major(bigger(track.second.first, track.second.second));
			bool state = _event.mouseButton.button == sf::Mouse::Left;
			sf::Color color;

			if (_event.mouseButton.button == sf::Mouse::Left)
				color = sf::Color(255U, 0U, 0U, 100U);
			else
				color = sf::Color(0U, 0U, 255U, 100U);

			for (i = minor.x; i <= major.x; i++)
				for (j = minor.y; j <= major.y; j++)
				{
					grid[i][j].obstacle = state;
					gridBlock[i][j].first.setFillColor(color);
				}

			track.second.first = sf::Vector2i(0, 0);
			track.second.second = sf::Vector2i(0, 0);
			track.first.setSize(sf::Vector2f(
				gridScale.x * track.second.second.x - track.second.first.x,
				gridScale.y * track.second.second.y - track.second.first.y
			));

			mousePressed = false;
		}
		else if (mousePressed)
		{
			track.second.second = GraphicManager::MouseToGrid();
			sf::Vector2i delta(
				track.second.second.x - track.second.first.x,
				track.second.second.y - track.second.first.y
			);
			sf::Vector2f minor(lesser(track.second.first, track.second.second));

			track.first.setSize(sf::Vector2f(
				gridScale.x * (abs(delta.x) + 1),
				gridScale.y * (abs(delta.y) + 1)
			));
			track.first.setPosition(
				gridScale.x * minor.x,
				gridScale.y * minor.y
			);
		}
	}
};

void Stage::Execute()
{
	unsigned int i = 0;

	for (i = 0; i < this->entities.GetSize(); i++)
	{
		this->entities[i]->Execute();
	}
	this->pColisionManager->UpdateColisions(elapsedTime);

	for (character::enemy::Enemy* en : enemies)
		en->UpdateTarget();

	GraphicManager::UpdateCamera();
	this->background.Execute();
};
void Stage::DebugExecute()
{
	unsigned int i = 0;

	for (i = 0; i < this->entities.GetSize(); i++)
	{
		this->entities[i]->DebugExecute();
	}
	this->pColisionManager->UpdateColisions(elapsedTime);

	for (character::enemy::Enemy* en : enemies)
		en->UpdateTarget();

	GraphicManager::UpdateCamera();
	this->background.Execute();
};
void Stage::SelfPrint()
{
	unsigned int i = 0;

	this->background.SelfPrint();
	for(i = 0; i < this->entities.GetSize(); i++)
	{
		this->entities[i]->SelfPrint();
	}
};
void Stage::DebugSelfPrint()
{
	unsigned int i = 0, j = 0, x = 0;
	sf::UIntRect bounds(GraphicManager::GridIndexsToCamera());

	this->background.SelfPrint();
	for (i = 0; i < this->entities.GetSize(); i++)
	{
		this->entities[i]->DebugSelfPrint();
	}
	GraphicManager::Draw(this->bounds);
	
	for(j = bounds.left; j < bounds.width && j < this->grid.size(); j++)
		for (x = bounds.top; x < bounds.height && x < this->grid[j].size(); x++)
		{
			GraphicManager::Draw(gridBlock[j][x].first);
			GraphicManager::Draw(gridBlock[j][x].second);
		}

	GraphicManager::Draw(this->track.first);
};

void Stage::OrganizeObstacles() 
{
	if (grid.size() < 1)
		return;
	if (grid[0].size() < 1)
		return;

	std::vector<std::vector<Obstacle::ObsType>> obsTxtType;
	std::vector<unsigned long long int> ids;
	sf::IntRect range(0, 0, grid.size(), grid[0].size());
	std::list<sf::Vector2i> ignoreList;

	sf::Vector2f pos(0.f, 0.f);
	sf::Vector2u size(0, 0);

	bool up = false, down = false, left = false, right = false;
	bool lUp = false, rUp = false, lDown = false, rDown = false;

	int i = 0, j = 0, h = 0, k = 0, x = 0, y = 0;
	int counter = 0;

	ids.reserve(this->entities.GetSize());
	for (unsigned int aux = 0; aux < this->entities.GetSize(); aux++)
		if (this->entities[aux]->GetType() == Type::OBSTACLE)
			ids.push_back(this->entities[aux]->GetId());
	
	this->RemoveEntity(ids);

	for (i = range.left; i < range.width; i++)
	{
		for (j = 0; j < range.height; j++)
		{
			// Elimina nodos que não mais seram analisados
			ignoreList.remove_if(
				[&](const sf::Vector2i& val) -> bool
				{
					return val.x < i || (val.x == i && val.y < j);
				}
			);

			// Ignora nodo caso já tenha sido coberto como um obstáculo
			for (sf::Vector2i& vec : ignoreList)
				if (vec.x == i && vec.y == j)
					goto OFF_MAJOR;

			if (grid[i][j].obstacle)
			{
				range.left = i;
				range.top = j;
				x = i;
				y = j;

				// verifica alcance do possível obstáculo no eixo X
				while (x < range.width && grid[x][j].obstacle)
					x++;
				range.width = x;

				// verifica alcance do possível�vel obstáculo no eixo Y
				while (y < range.height && grid[i][y].obstacle)
				{
					y++;

					// Ignora nodo abaixo do novo poss�vel obstáculo caso j� tenha sido coberto como obstáculo
					for (sf::Vector2i& vec : ignoreList)
						if (vec.x == i && vec.y == y)
							goto OFF_MINOR;
				}
			OFF_MINOR:;
				range.height = y;

				// Reduz tamanho do obstáculo caso haja n�o obstáculos dentro de sua �rea
				for (x = i + 1; x < range.width; x++)
					for (y = j + 1; y < range.height; y++)
						if (!grid[x][y].obstacle)
							range.height = y;

				// Para a �rea do novo obstáculo defini os nodos � n�o serem mais analisados
				for (x = range.left + 1; x < range.width; x++)
					for (y = range.top; y < range.height; y++)
						ignoreList.emplace_back(x, y);

				obsTxtType.resize(range.width - range.left);
				for (std::vector<Obstacle::ObsType>& vec : obsTxtType)
					vec.resize(range.height - range.top, Obstacle::ObsType::NEUTRAL);

				for (h = range.left; h < range.width; h++)
					for (k = range.top; k < range.height; k++)
					{
						counter = 0;
						up		= false;
						down	= false;
						left	= false;
						right	= false;
						lUp		= false;
						rUp		= false;
						lDown	= false;
						rDown	= false;

						if (k == 0)
						{
							up = true;
							counter += 3;
						}
						else if (k == grid[0].size() - 1)
						{
							down = true;
							counter += 3;
						}
						if (h == 0)
						{
							left = true;
							counter += 3;
						}
						else if (h == grid.size() - 1)
						{
							right = true;
							counter += 3;
						}

						for (x = h - (h > 0); x <= (h + ((unsigned)h < grid.size() - 1)); x++)
							for (y = k - (k > 0); y <= (k + ((unsigned)k < grid[0].size() - 1)); y++)
							{
								if (!grid[x][y].obstacle || (x == h && y == k))
									continue;

								if (x == h && y < k)
									up = true;
								else if (x == h && y > k)
									down = true;
								else if (x < h && y == k)
									left = true;
								else if (x > h && y == k)
									right = true;
								else if (x < h && y < k)
									lUp = true;
								else if (x > h && y < k)
									rUp = true;
								else if (x < h && y > k)
									lDown = true;
								else if (x > h && y > k)
									rDown = true;

								counter++;
							}

						x = h - range.left;
						y = k - range.top;

						// verifica se é um bloco de quina
						if (up && left && !down && !right)
							obsTxtType[x][y] = Obstacle::ObsType::RIGHT_BOTTOM_EDGE;
						else if (up && right && !down && !left)
							obsTxtType[x][y] = Obstacle::ObsType::LEFT_BOTTOM_EDGE;
						else if (down && left && !up && !right)
							obsTxtType[x][y] = Obstacle::ObsType::RIGHT_TOP_EDGE;
						else if (down && right && !up && !left)
							obsTxtType[x][y] = Obstacle::ObsType::LEFT_TOP_EDGE;
						// verifica se é um bloco de quina interna
						else if (up && right && down && left && lUp && rUp && lDown && !rDown)
							obsTxtType[x][y] = Obstacle::ObsType::INNER_RIGHT_BOTTOM;
						else if (up && right && down && left && lUp && rUp && !lDown && rDown)
							obsTxtType[x][y] = Obstacle::ObsType::INNER_LEFT_BOTTOM;
						else if (up && right && down && left && lUp && !rUp && lDown && rDown)
							obsTxtType[x][y] = Obstacle::ObsType::INNER_RIGHT_TOP;
						else if (up && right && down && left && !lUp && rUp && lDown && rDown)
							obsTxtType[x][y] = Obstacle::ObsType::INNER_LEFT_TOP;
						// verifica se é um bloco de aresta
						else if (right && left && down && !up)
							obsTxtType[x][y] = Obstacle::ObsType::TOP;
						else if (right && left && up && !down)
							obsTxtType[x][y] = Obstacle::ObsType::BOTTOM;
						else if (up && down && left && !right)
							obsTxtType[x][y] = Obstacle::ObsType::RIGHT;
						else if (up && down && right && !left)
							obsTxtType[x][y] = Obstacle::ObsType::LEFT;
						// caso contrário é um bloco do meio
						else if (counter <= 6)
							obsTxtType[x][y] = Obstacle::ObsType::NEUTRAL;
						else
							obsTxtType[x][y] = Obstacle::ObsType::MIDDLE;
					}

				pos = sf::Vector2f(gridScale.x * range.left, gridScale.y * range.top);
				size = sf::Vector2u(range.width - range.left, range.height - range.top);

				this->EntityCreated(new obstacle::Tile(size, pos, obsTxtType));
				j = range.height - 1;

				range.width = grid.size();
				range.height = grid[0].size();
			}
		OFF_MAJOR:;
		}
	}

	bool vasco = true;
};
void Stage::AddPlayer()
{};
void Stage::RemovePlayer()
{};

void Stage::VerifyGrid() 
{
	unsigned int i = 0, j = 0;

	for(i = 0; i < grid.size(); i++)
	{
		for (j = 0; j < grid[i].size(); j++)
		{
			if (grid[i][j].obstacle)
				gridBlock[i][j].first.setFillColor(sf::Color(255U, 0U, 0U, 100U));
			else
				gridBlock[i][j].first.setFillColor(sf::Color(0U, 0U, 255U, 100U));
		}
	}
};
void Stage::VerifyGridOcupation()
{
	unsigned int i = 0, j = 0, x = 0;
	sf::FloatRect sizeInGrid;
	sf::UIntRect LimToGrid;

	for (i = 0; i < this->entities.GetSize(); i++)
	{
		if (this->entities[i]->GetType() == Type::OBSTACLE)
		{
			sizeInGrid = this->entities[i]->GetBounds();
			LimToGrid = sf::UIntRect(
				unsigned int(roundf(sizeInGrid.left		/ this->gridScale.x)),
				unsigned int(roundf(sizeInGrid.top		/ this->gridScale.y)),
				unsigned int(roundf(sizeInGrid.width	/ this->gridScale.x)),
				unsigned int(roundf(sizeInGrid.height	/ this->gridScale.y))
			);

			for (j = LimToGrid.left; j < LimToGrid.width; j++)
			{
				for (x = LimToGrid.top; x < LimToGrid.height; x++)
					grid[j][x].obstacle = true;
			}
		}
	}
};
void Stage::DebugVerifyGridOcupation()
{
	unsigned int i = 0, j = 0, x = 0;
	sf::FloatRect sizeInGrid;
	sf::UIntRect LimToGrid;
	Entity* aux = nullptr;

	for (i = 0; i < this->entities.GetSize(); i++)
	{
		aux = this->entities[i];

		if (aux->GetType() == Type::OBSTACLE)
		{
			sizeInGrid = aux->GetBounds();
			LimToGrid = sf::UIntRect(
				unsigned int(roundf(sizeInGrid.left		/ this->gridScale.x)),
				unsigned int(roundf(sizeInGrid.top		/ this->gridScale.y)),
				unsigned int(roundf(sizeInGrid.width	/ this->gridScale.x)),
				unsigned int(roundf(sizeInGrid.height	/ this->gridScale.y))
			);

			for (j = LimToGrid.left; j < LimToGrid.width && j < grid.size(); j++)
			{
				for (x = LimToGrid.top; x < LimToGrid.height && x < grid[j].size(); x++)
				{
					grid[j][x].obstacle = true;
					gridBlock[j][x].first.setFillColor(sf::Color(255U, 0U, 0U, 100U));
				}
			}
		}
	}
};

void Stage::EntityCreated(Entity* pEntity)
{
	if (pEntity == nullptr)
		return;

	this->entities.AddEntity(pEntity);
	this->pColisionManager->Add(pEntity);

	if (pEntity->GetType() == Type::ENEMY)
		this->enemies.emplace_back(static_cast<character::enemy::Enemy*>(pEntity));
};
void Stage::RemoveEntity(unsigned long long int id)
{
	for (character::enemy::Enemy*& pEne : enemies)
		if (pEne->GetId() == id)
		{
			enemies.remove(pEne);
			break;
		}

	this->pColisionManager->Remove(id);
	this->entities.RemoveEntity(id);
};
void Stage::RemoveEntity(const std::vector<unsigned long long int>& ids)
{
	for(const unsigned long long int& id : ids)
		for (character::enemy::Enemy*& pEne : enemies)
			if (pEne->GetId() == id)
			{
				enemies.remove(pEne);
				break;
			}

	this->pColisionManager->RemoveRange(ids);
	this->entities.RemoveRange(ids);
};