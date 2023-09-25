#pragma once
#include "global-includes.h"
#include "entity.h"
#include "world.h"
#include "hostile.h"
#include "localplayer.h"
#include "entitymanager.h"

class EntityManager
{
public:
	EntityManager();
	Entity* AddEntity(std::unique_ptr<Entity> entity);
	void RenderEntities(sf::RenderWindow& window);
	void ProcessEntityLogic(float frameTime);
	void RemoveEntity(size_t index);
	Entity* GetEntity(size_t index);
	void ResetAll();
	
	World* CreateWorld();
	World* GetWorld()
	{
		return m_world.get();
	}

	void DeleteMarkedEntities();

	LocalPlayer* CreateLocalPlayer();
	LocalPlayer* GetLocalPlayer()
	{
		return m_localplayer.get();
	}

	size_t GetHighestEntityIndex()
	{
		return m_highest_entity_index;
	}
private:
	std::vector<std::unique_ptr<Entity>> m_entities;
	std::unique_ptr<LocalPlayer> m_localplayer;
	std::unique_ptr<World> m_world;
	size_t m_highest_entity_index;
};

extern std::unique_ptr<EntityManager> entityManager;