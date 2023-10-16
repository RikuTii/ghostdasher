#pragma once
#include "world.h"


class PathFinder
{
public:
	PathFinder() { };
	PathFinder(World* world) : m_world(world), m_iteration_count(400), m_adjust_count(5), m_move_step(3.0f), m_gen_mode(Normal), m_adjust_step(0.6f){ };

	std::vector<sf::Vector2f> GenerateBestPath(const sf::Vector2f& entity, const sf::Vector2f&, const sf::Vector2f&);
	std::vector<sf::Vector2f> GenerateBestPathAStar(const sf::Vector2f&, const sf::Vector2f&);

	std::vector<sf::Vector2f> GenerateHorizontalPath(const sf::Vector2f&, const sf::Vector2f&);
	std::vector<sf::Vector2f> GenerateVerticalPath(const sf::Vector2f&, const sf::Vector2f&);
	int AdjustMovement(const sf::Vector2f&, const sf::Vector2f&, sf::Vector2f&);

	enum GenerateMode
	{
		Normal,
		Negative,
		Positive
	};

	void SetGenMode(GenerateMode mode)
	{
		m_gen_mode = mode;
	}

private:
	std::unique_ptr<sf::VertexArray> m_draw;
	World* m_world;
	sf::Vector2f m_tracking_entity;
	size_t m_iteration_count;
	float m_move_step;
	float m_adjust_step;
	size_t m_adjust_count;
	GenerateMode m_gen_mode;

};

extern std::unique_ptr<PathFinder> pathFinder;