#include "resourcemanager.h"


std::unique_ptr<ResourceManager> m_resourceManager;

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
}