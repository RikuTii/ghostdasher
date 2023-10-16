#pragma once
#include "global-includes.h"
#include "entity.h"
#include "world.h"
#include "hostile.h"
#include "localplayer.h"

class EntityManager
{
public:
	EntityManager();
	void RenderEntities(sf::RenderWindow& window);
	void ProcessEntityLogic(float frameTime);
	void RemoveEntity(size_t index);
	Entity* GetEntity(size_t index);
	void ResetAll();
	void DeleteMarkedEntities();
	LocalPlayer* CreateLocalPlayer();
	World* CreateWorld();

	template<class T>
	std::vector<T> GetEntitiesByType()
	{
		std::vector<T> m_ents;
		for (const auto& it : m_entities)
		{
			if (it && !it->GetMarkedForDeletion() && it.get())
			{
				auto ent = dynamic_cast<T>(it.get());
				if (ent)
				{
					m_ents.push_back(ent);
				}
			}
			
		}

		return m_ents;
	}

	template<class T>
	T* AddEntity(std::unique_ptr<T> m_entity)
	{
		m_entities.emplace_back(std::move(m_entity));

		m_entities.back().get()->SetEntityIndex(m_entities.size());

		m_highest_entity_index = m_entities.size() + 1;

		return dynamic_cast<T*>(m_entities.back().get());
	}



	World* GetWorld()
	{
		if (m_world)
		{
			return m_world.get();
		}

		return nullptr;
	}

	LocalPlayer* GetLocalPlayer()
	{
		return m_localplayer.get();
	}

	size_t GetHighestEntityIndex()
	{
		return m_entities.size();
	}
private:
	std::vector<std::unique_ptr<Entity>> m_entities;
	std::unique_ptr<LocalPlayer> m_localplayer;
	std::unique_ptr<World> m_world;
	size_t m_highest_entity_index;
};

extern std::unique_ptr<EntityManager> entityManager;