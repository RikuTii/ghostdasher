#include "entitymanager.h"
std::unique_ptr<EntityManager> entityManager;

EntityManager::EntityManager()
{
	m_highest_entity_index = 0;
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

	auto it = find_if(m_entities.begin(), m_entities.end(), [&](auto const &obj) { return obj && obj->GetEntityIndex() == index; });
	if (it != m_entities.end())
	{
		m_entities.erase(it);
	}
}




void EntityManager::ResetAll()
{
	m_entities.clear();
	m_highest_entity_index = 0;
}

Entity* EntityManager::GetEntity(size_t index)
{

	auto it = find_if(m_entities.begin(), m_entities.end(), [&](auto const& obj) { return obj && obj->GetEntityIndex() == index; });
	if (it != m_entities.end()) 
	{
		return it->get();
	}

	return nullptr;
}



void EntityManager::DeleteMarkedEntities()
{
	std::erase_if(m_entities,
		[=](auto const& ptr) { return ptr && ptr->GetMarkedForDeletion() == true; });
}

void EntityManager::RenderEntities(sf::RenderWindow& window)
{
	for (auto const& it : m_entities)
	{
		if (m_world->IsBossFight())
		{
			if (it->GetCategory() == EntityCategory::CategoryBoss || it->GetCategory() == EntityCategory::CategoryAlwaysActive)
			{
				it->Render(window);
			}
		}
		else
		{
			it->Render(window);
		}
	}
}

void EntityManager::ProcessEntityLogic(float frameTime)
{
	for (auto const & it: m_entities)
	{
		if (it)
		{
			if (m_world->IsBossFight())
			{
				if (it->GetCategory() == EntityCategory::CategoryBoss || it->GetCategory() == EntityCategory::CategoryAlwaysActive)
				{
					it->Process(frameTime);
				}
			}
			else
			{
				it->Process(frameTime);
			}
		}
	}
}