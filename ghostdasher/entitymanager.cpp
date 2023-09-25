#include "entitymanager.h"
std::unique_ptr<EntityManager> entityManager;

EntityManager::EntityManager()
{

}

Entity* EntityManager::AddEntity(std::unique_ptr<Entity> m_entity)
{
	m_entities.emplace_back(std::move(m_entity));

	m_entities.back().get()->SetEntityIndex(m_entities.size());

	m_highest_entity_index = m_entities.size() + 1;

	return m_entities.back().get();
}


World* EntityManager::CreateWorld()
{
	m_world = std::make_unique<World>();
	return m_world.get();
}



LocalPlayer* EntityManager::CreateLocalPlayer()
{
	m_localplayer = std::make_unique<LocalPlayer>();
	return m_localplayer.get();
}


void EntityManager::RemoveEntity(size_t index)
{

	auto it = find_if(m_entities.begin(), m_entities.end(), [&](auto const &obj) { return obj->GetEntityIndex() == index; });
	if (it != m_entities.end())
	{
		m_entities.erase(it);
	}
}




void EntityManager::ResetAll()
{
	m_entities.clear();
}

Entity* EntityManager::GetEntity(size_t index)
{

	auto it = find_if(m_entities.begin(), m_entities.end(), [&](auto const& obj) { return obj->GetEntityIndex() == index; });
	if (it != m_entities.end()) 
	{
		return it->get();
	}

	return nullptr;
}



void EntityManager::DeleteMarkedEntities()
{
	std::erase_if(m_entities,
		[=](auto const& ptr) { return ptr->GetMarkedForDeletion() == true; });
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