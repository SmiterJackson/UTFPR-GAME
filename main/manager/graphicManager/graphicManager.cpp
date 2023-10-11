#include "graphicManager.h"
#include "ente/entity/obstacle/obstacle.h"
#include "ente/entity/character/enemy/enemy.h"
using namespace manager;
using namespace obstacle;
using namespace character;
using namespace character::enemy;

#define PATH "resources/fonts/EquipmentPro.ttf"

#define CAMERA_ZOOM 2.f

#define WINDOW_SIZE sf::Vector2i(1440, 810)
#define VIEW_SIZE sf::Vector2f(1440.f / CAMERA_ZOOM, 810.f / CAMERA_ZOOM)

#define DISTORTION_X true
#define DISTORTION_Y true
#define BLCK_BAR_X true
#define BLCK_BAR_Y true

#define OFF_CAMERA_EXTRA_SPACE_COEFF 0.5f

const float         GraphicManager::cameraExtraSpace    = OFF_CAMERA_EXTRA_SPACE_COEFF;
TextureMap*         GraphicManager::textures            = new TextureMap();
GraphicManager*     GraphicManager::instance            = nullptr;
trait::GameState*   GraphicManager::pState              = nullptr;
sf::Font*           GraphicManager::font                = nullptr;
sf::RenderWindow*   GraphicManager::window              = nullptr;
sf::FloatRect       GraphicManager::cameraLim           = sf::FloatRect();
sf::View            GraphicManager::view                = sf::View(sf::Vector2f(), VIEW_SIZE);
sf::Vector2f        GraphicManager::gridScale           = sf::Vector2f();

GraphicManager* GraphicManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new GraphicManager();
        if (instance == nullptr)
            std::cerr << "Nao foi possivel instanciar um gerenciador grafico: graphic_manager." << std::endl;
    }

    return instance;
};
void GraphicManager::DeconsInstance()
{
    if (instance != nullptr)
        delete instance;
};

GraphicManager::GraphicManager() :
    originalSize(VIEW_SIZE),
    zoom(CAMERA_ZOOM), 
    distort_x(DISTORTION_X), 
    distort_y(DISTORTION_Y),
    bar_x(BLCK_BAR_X), 
    bar_y(BLCK_BAR_Y)
{
    window = new sf::RenderWindow(sf::VideoMode(WINDOW_SIZE.x, WINDOW_SIZE.y), "JANELA DE CONTEXTO");
    window->setKeyRepeatEnabled(false);
    window->setFramerateLimit(60);

    if(textures == nullptr)
        textures = new TextureMap();

    if (font == nullptr)
    {
        font = new sf::Font();

        if (font != nullptr)
            font->loadFromFile(PATH);
        else
            std::cerr << "Nao foi possivel alocar uma fonte de texto: graphic_manager." << std::endl;
    }
};
GraphicManager::~GraphicManager()
{
    if (window != nullptr)
        delete window;

    if (font != nullptr)
        delete font;
};

void GraphicManager::UpdateCamera()
{
    std::list<Player*> pList = Player::GetPlayerList();
    sf::Vector2f diff, radious(view.getSize() / 2.f);
    sf::Vector2f least, most;
    sf::Vector2f delta(0.f, 0.f);

    constexpr float max = std::numeric_limits<float>::max();
    constexpr float min = std::numeric_limits<float>::lowest();

    // Retira o centro entre todas as int�ncias de Player
    if (pList.size() > 0)
    {
        least = sf::Vector2f(max, max);
        most = sf::Vector2f(min, min);

        for (Player* p : pList)
        {
            delta = p->GetPosition();
            /*
            if (pos.x < least.x)
                least.x = pos.x;
            if (pos.y < least.y)
                least.y = pos.y;

            if (pos.x > most.x)
                most.x = pos.x;
            if (pos.y > most.y)
                most.y = pos.y;
            */
            // versao branchless
            least = sf::Vector2f(
                (least.x * (delta.x > least.x)) + (delta.x * (delta.x <= least.x)),
                (least.y * (delta.y > least.y)) + (delta.y * (delta.y <= least.y))
            );
            most = sf::Vector2f(
                (most.x * (delta.x < most.x)) + (delta.x * (delta.x >= most.x)),
                (most.y * (delta.y < most.y)) + (delta.y * (delta.y >= most.y))
            );
        }

        diff = most - least;
        diff /= 2.f;

        least += diff;
    }

    if (cameraLim.left != cameraLim.width &&
        cameraLim.top  != cameraLim.height)
    {
        delta = least - radious;
        /*
        if (pos.x < cameraLim.left)
        {
            if (cameraLim.left <= 0)
                least.x += fabs(pos.x - cameraLim.left);
            else
                least.x += pos.x - cameraLim.left;
        }
        if (pos.y < cameraLim.top)
        {
            if (cameraLim.top <= 0)
                least.y += fabs(pos.y - cameraLim.top);
            else
                least.y += pos.y - cameraLim.top;
        }
        */
        // versao branchless
        least = sf::Vector2f(
            least.x + ((delta.x < cameraLim.left) * (
                            (fabs(delta.x - cameraLim.left) * (cameraLim.left <= 0)) + 
                            (    (delta.x - cameraLim.left) * (cameraLim.left >  0))
                        )),
            least.y + ((delta.y < cameraLim.top) * (
                            (fabs(delta.y - cameraLim.top) * (cameraLim.top <= 0)) +
                            (    (delta.y - cameraLim.top) * (cameraLim.top >  0))
                        ))
        );

        delta = least + radious;
        /*
        if (pos.x > cameraLim.width)
            least.x -= pos.x - cameraLim.width;
        if (pos.y > cameraLim.height)
            least.y -= pos.y - cameraLim.height;
        */
        // versao branchless
        least = sf::Vector2f(
            least.x - ((delta.x - cameraLim.width)  * (delta.x > cameraLim.width)),
            least.y - ((delta.y - cameraLim.height) * (delta.y > cameraLim.height))
        );
    }

    view.setCenter(least);
};
void GraphicManager::WindowResize() const
{
    sf::FloatRect newPort(0.f, 0.f, 1.f, 1.f);
    sf::Vector2f newSize(this->originalSize.x, this->originalSize.y);

    // Propor��o do novo tamanho de tela em fun��o do tamanho anterior
    sf::Vector2f ratio(
        window->getSize().x / this->zoom / this->originalSize.x,
        window->getSize().y / this->zoom / this->originalSize.y
    );

    if (this->distort_x)
    {
        if (this->bar_x && ratio.x > 1.f)
        {
            ratio.x = 1.f / ratio.x;
            ratio.x = 1.f - ratio.x;

            newPort.left += ratio.x / 2.f;
            newPort.width -= ratio.x;
        }
    }
    else
        newSize.x = this->originalSize.x * ratio.x;

    if (this->distort_y)
    {
        if (this->bar_y && ratio.y > 1.f)
        {
            ratio.y = 1.f / ratio.y;
            ratio.y = 1.f - ratio.y;

            newPort.top += ratio.y / 2.f;
            newPort.height -= ratio.y;
        }
    }
    else
        newSize.y = this->originalSize.y * ratio.y;

    view.setSize(newSize);
    view.setViewport(newPort);
};

const std::list<Obstacle*> GraphicManager::GetCameraEntities(const std::list<Obstacle*>& entities)
{
    std::list<Obstacle*>::const_iterator cIt;
    std::list<Obstacle*> entitiesInCam;

    sf::Vector2f entSize;
    float delta_X = 0;

    for (cIt = entities.cbegin(); cIt != entities.cend(); cIt++)
    {
        entSize = (*cIt)->GetSize() / 2.f;
        delta_X = fabs(GetViewPosition().x - (*cIt)->GetPosition().x);

        if (delta_X <= (GetViewSize().x / 2.f * (1.f + cameraExtraSpace)) + entSize.x)
            entitiesInCam.emplace_back((*cIt));
    }

    return entitiesInCam;
};
const std::list<Enemy*> GraphicManager::GetCameraEntities(const std::vector<Enemy*>& entities)
{
    std::vector<Enemy*>::const_iterator cIt;
    std::list<Enemy*> entitiesInCam;

    sf::Vector2f entSize;
    float delta_X = 0;

    for (cIt = entities.cbegin(); cIt != entities.cend(); cIt++)
    {
        entSize = (*cIt)->GetSize() / 2.f;
        delta_X = fabs(GetViewPosition().x - (*cIt)->GetPosition().x);

        if (delta_X <= (GetViewSize().x / 2.f * (1.f + cameraExtraSpace)) + entSize.x)
            entitiesInCam.emplace_back((*cIt));
    }

    return entitiesInCam;
};

void GraphicManager::Draw(const sf::RectangleShape& drawTarget)
{
    window->draw(drawTarget);
};
void GraphicManager::Draw(const sf::CircleShape& drawTarget)
{
    window->draw(drawTarget);
};
void GraphicManager::Draw(const sf::Sprite& drawTarget)
{
    window->draw(drawTarget);
};
void GraphicManager::Draw(const sf::Text& drawTarget)
{
    window->draw(drawTarget);
};

sf::Texture* GraphicManager::LoadTexture(std::string texturePath, sf::IntRect sheetCut, bool repeated)
{
    std::stringstream sstring;
    sf::Texture* newTexture = nullptr;

    sstring << texturePath << sheetCut.left << 'x' << sheetCut.width << ':' << sheetCut.top << 'x' << sheetCut.height;

    if (texturePath == "resources/textures/tile_sheet/tile_sheet.png")
        bool sex = true;

    if(!textures->empty())
        for (TextureMap::value_type vlt : *textures)
        {
            if (vlt.first == texturePath || vlt.first == sstring.str())
                return vlt.second;
        }

    newTexture = new sf::Texture();
    if (newTexture == nullptr)
    {
        std::cerr << "Nao foi possivel instanciar uma nova textura: " << texturePath << std::endl;
        return newTexture;
    }

    if (sheetCut.width != 0 && sheetCut.height != 0)
    {
        newTexture->loadFromFile(texturePath, sheetCut);
        newTexture->setRepeated(repeated);
        textures->insert(TextureMap::value_type(sstring.str(), newTexture));
    }
    else
    {
        newTexture->loadFromFile(texturePath);
        newTexture->setRepeated(repeated);
        textures->insert(TextureMap::value_type(texturePath, newTexture));
    }

    return newTexture;
};
void GraphicManager::Update()
{
    window->clear();

    pState->Print();

    window->setView(view);
    window->display();
};
