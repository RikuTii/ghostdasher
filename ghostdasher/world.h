#pragma once
#include "global-includes.h"
#include "entity.h"
#include <tmxlite/Map.hpp>

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

struct GeneratedObject
{
	std::string m_name;
	sf::FloatRect m_bounds;
	std::unique_ptr<sf::RectangleShape> m_shape;
	bool m_generated;
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
	int GetIntersection(const sf::Vector2f&, const sf::Vector2f& ent = sf::Vector2f(0,0), bool first = false);
	void AddUnwalkableSpace(const sf::FloatRect&);
	void AddSpawnPoint(const sf::Vector2f&);
	bool IsPointVisible(const sf::Vector2f& start, const sf::Vector2f& end);
	bool IsInViewBounds(const sf::Vector2f&);
	std::vector<sf::FloatRect> GetUnwalkableSpaces();
	sf::Vector2f GetRandomSpawnPoint();
	sf::Vector2f GetFurthestSpawnpoint(const sf::Vector2f&);
	void LoadPotentialObject(const tmx::Object&);
	void RenderExtra(sf::RenderWindow& renderWindow);

	void SetBounds(const sf::Vector2f& bounds)
	{
		m_bounds = bounds;
		m_shape->setSize(m_bounds);
	}
	void UpdateIntersectingEntities();

	std::vector<Waypoint> GenerateWayPoints();

	void UpdateUnwalkableSpaces();

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

	void SetBossPosition(const sf::Vector2f& pos)
	{
		m_boss_position = pos;
	}

	const sf::Vector2f& GetBossPosition()
	{
		return m_boss_position;
	}

	void TriggerBossfight()
	{
		m_is_boss_fight = true;
	}

	bool IsBossFight()
	{
		return m_is_boss_fight;
	}

private:
	std::unique_ptr<sf::RectangleShape> m_shape;
	std::vector<sf::FloatRect> m_unwalkable_spaces;
	std::vector<sf::FloatRect> m_dynamic_unwalkable_spaces;
	std::vector<Waypoint> m_points;
	std::vector<sf::Vector2f> m_spawnpoints;
	std::vector<sf::FloatRect> m_intersecting_entity_positions;

	std::vector<std::unique_ptr<GeneratedObject>> m_potential_objects;

	sf::Vector2f m_boss_position;
	sf::Vector2f m_view_position;
	sf::Vector2f m_view_size;
	sf::Vector2f m_position;
	sf::Vector2f m_bounds;
	float m_waypoint_size;
	bool m_is_boss_fight;
};