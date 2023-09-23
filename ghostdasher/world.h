#pragma once
#pragma once
#include "global-includes.h"
#include "entity.h"

class World : public Entity
{
public:
	World();
	void Render(sf::RenderWindow& renderWindow);
	void SetPosition(const sf::Vector2f& pos);
	void Process(float frameTime);
	void InitalizeWalkableSpace();
	bool DoesIntersectWall(const sf::FloatRect&);
	void TryMovement(Entity* ent);

	int GetIntersection(const sf::Vector2f&);

	void AddUnwalkableSpace(const sf::FloatRect&);
	void AddSpawnPoint(const sf::Vector2f&);

	sf::Vector2f GetRandomSpawnPoint();


	sf::Vector2f GetWorldBounds()
	{
		return m_bounds;
	}
private:
	std::unique_ptr<sf::RectangleShape> m_shape;
	std::vector<sf::FloatRect> m_unwalkable_spaces;
	std::vector<sf::Vector2f> m_spawnpoints;
	sf::Vector2f m_position;
	sf::Vector2f m_bounds;
};