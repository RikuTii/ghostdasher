#pragma once
#include "global-includes.h"
#include "entity.h"

struct Waypoint
{
	int id;
	bool m_walkable;
	sf::Vector2f m_position;
	int x;
	int y;
	int parent_id;
	int g_cost;
	int h_cost;
	int f_cost()
	{
		return g_cost + h_cost;
	}
};
class World : public Entity
{
public:
	World();
	void Render(sf::RenderWindow& renderWindow);
	void SetPosition(const sf::Vector2f& pos);
	void Process(float frameTime);
	bool DoesIntersectWall(const sf::FloatRect&);
	bool TryMovement(Entity* ent);
	int GetIntersection(const sf::Vector2f&, Entity*ent = nullptr, bool first = false);
	void AddUnwalkableSpace(const sf::FloatRect&);
	void AddSpawnPoint(const sf::Vector2f&);
	bool IsPointVisible(const sf::Vector2f& start, const sf::Vector2f& end);
	bool IsInViewBounds(const sf::Vector2f&);
	std::vector<sf::FloatRect> GetUnwalkableSpaces();
	sf::Vector2f GetRandomSpawnPoint();
	sf::Vector2f GetFurthestSpawnpoint(const sf::Vector2f&);

	std::vector<Waypoint> GenerateWayPoints();

	void UpdateView(const sf::Vector2f& pos, const sf::Vector2f& size)
	{
		m_view_position = pos;
		m_view_size = size;
	}

	sf::Vector2f GetWorldBounds()
	{
		return m_bounds;
	}

	int GetWaypointsWidth()
	{
		return static_cast<int>(GetWorldBounds().x / m_waypoint_size);
	}

private:
	std::unique_ptr<sf::RectangleShape> m_shape;
	std::vector<sf::FloatRect> m_unwalkable_spaces;
	std::vector<Waypoint> m_points;
	std::vector<sf::Vector2f> m_spawnpoints;
	sf::Vector2f m_view_position;
	sf::Vector2f m_view_size;
	sf::Vector2f m_position;
	sf::Vector2f m_bounds;
	float m_waypoint_size;
};