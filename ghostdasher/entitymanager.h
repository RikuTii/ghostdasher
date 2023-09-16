#pragma once
#include "global-includes.h"
#include "entity.h"
#include "world.h"

class EntityManager
{
public:
	EntityManager();

	Entity* AddEntity(std::unique_ptr<Entity> entity);
	void RenderEntities(sf::RenderWindow& window);
	void ProcessEntityLogic(float frameTime);
	
	World* CreateWorld();
	World* GetWorld()
	{
		return m_world.get();
	}
private:
	std::vector<std::unique_ptr<Entity>> m_entities;
	std::unique_ptr<World> m_world;
};