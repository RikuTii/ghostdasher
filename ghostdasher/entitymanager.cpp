#include "entitymanager.h"

EntityManager::EntityManager()
{

}

Entity* EntityManager::AddEntity(std::unique_ptr<Entity> m_entity)
{
	m_entities.push_back(std::move(m_entity));

	return m_entities.back().get();
}


World* EntityManager::CreateWorld()
{
	m_world = std::make_unique<World>();
	return m_world.get();
}



void EntityManager::RenderEntities(sf::RenderWindow& window)
{
	for (auto const& it : m_entities)
	{
		it->Render(window);
	}
}

void EntityManager::ProcessEntityLogic(float frameTime)
{
	for (auto const & it: m_entities)
	{
		it->Process(frameTime);
	}
}