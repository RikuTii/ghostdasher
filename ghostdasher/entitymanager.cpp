#include "entitymanager.h"

EntityManager::EntityManager()
{

}

std::shared_ptr<Entity> EntityManager::AddEntity(std::shared_ptr<Entity> m_entity)
{
	m_entities.push_back(m_entity);

	return m_entity;
}


void EntityManager::RenderEntities(sf::RenderWindow& window)
{
	for (auto it : m_entities)
	{
		it->Render(window);
	}
}

void EntityManager::ProcessEntityLogic(float frameTime)
{
	for (auto it : m_entities)
	{
		it->Process(frameTime);
	}
}