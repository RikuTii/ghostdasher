#include "resourcemanager.h"


std::unique_ptr<ResourceManager> resourceManager;

ResourceManager::ResourceManager()
{

}
ResourceManager::~ResourceManager()
{

}

void ResourceManager::Loadresources()
{
    auto font = std::make_unique<sf::Font>();
    if (font->loadFromFile("C:/Users/juust/source/repos/ghostdasher/assets/tahoma.ttf"))
    {
        m_fonts.insert({ "Tahoma", std::move(font) });
    }

    {
        auto texture = std::make_unique<sf::Texture>();
        if (texture->loadFromFile("../assets/player/Run.png"))
        {
            m_textures.insert({ "LocalRun", std::move(texture) });
        }
    }
    {
        auto texture = std::make_unique<sf::Texture>();
        if (texture->loadFromFile("../assets/player/Idle.png"))
        {
            m_textures.insert({ "LocalIdle", std::move(texture) });
        }
    }
}