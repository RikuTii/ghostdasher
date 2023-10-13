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
	bool DoesIntersectWall(const sf::FloatRect&);
	void TryMovement(Entity* ent);
	int GetIntersection(const sf::Vector2f&, Entity*ent = nullptr);
	void AddUnwalkableSpace(const sf::FloatRect&);
	void AddSpawnPoint(const sf::Vector2f&);
	bool IsPointVisible(const sf::Vector2f& start, const sf::Vector2f& end);
	bool IsInViewBounds(const sf::Vector2f&);
	std::vector<sf::FloatRect> GetUnwalkableSpaces();
	sf::Vector2f GetRandomSpawnPoint();
	sf::Vector2f GetFurthestSpawnpoint(const sf::Vector2f&);

	void UpdateView(const sf::Vector2f& pos, const sf::Vector2f& size)
	{
		m_view_position = pos;
		m_view_size = size;
	}

	sf::Vector2f GetWorldBounds()
	{
		return m_bounds;
	}
private:
	std::unique_ptr<sf::RectangleShape> m_shape;
	std::vector<sf::FloatRect> m_unwalkable_spaces;
	std::vector<sf::Vector2f> m_spawnpoints;
	sf::Vector2f m_view_position;
	sf::Vector2f m_view_size;
	sf::Vector2f m_position;
	sf::Vector2f m_bounds;
};