#pragma once
#include "global-includes.h"

class Entity
{
public:
	Entity();
	void Render(sf::RenderWindow& renderWindow);
	void SetPosition(const sf::Vector2f& pos);
	void Process(float frameTime);
	void SetVelocity(const sf::Vector2f& vel);
	sf::Vector2f GetPosition()
	{
		return m_position;
	}
private:
	std::unique_ptr<sf::RectangleShape> m_shape;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_velocityGoal;
};