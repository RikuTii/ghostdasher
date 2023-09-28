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
    if (font->loadFromFile("../assets/tahoma.ttf"))
    {
        m_fonts.insert({ "Tahoma", std::move(font) });
    }

    {
        auto texture = std::make_unique<sf::Texture>();
        if (texture->loadFromFile("../assets/player/witchKitty_walk.png"))
        {
            m_textures.insert({ "LocalRun", std::move(texture) });
        }
    }
    {
        auto texture = std::make_unique<sf::Texture>();
        if (texture->loadFromFile("../assets/player/witchKitty_curiousIdleBreaker.png"))
        {
            m_textures.insert({ "LocalIdle", std::move(texture) });
        }
    }
    {
        auto texture = std::make_unique<sf::Texture>();
        if (texture->loadFromFile("../assets/enemy/Girl-Sheet.png"))
        {
            m_textures.insert({ "HostileIdle", std::move(texture) });
        }
    }
}