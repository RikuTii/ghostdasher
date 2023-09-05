#pragma once
#include "global-includes.h"
#include "entity.h"

class EntityManager
{
public:
	EntityManager();

	std::shared_ptr<Entity> AddEntity(std::shared_ptr<Entity> entity);
	void RenderEntities(sf::RenderWindow& window);
	void ProcessEntityLogic(float frameTime);
private:
	std::vector<std::shared_ptr<Entity>> m_entities;
};