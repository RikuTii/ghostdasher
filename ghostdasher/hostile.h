#pragma once
#include "global-includes.h"
#include "entity.h"

class Hostile : public Entity
{
public:
	Hostile();
	~Hostile() {};
	void Render(sf::RenderWindow& renderWindow);
	void SetPosition(const sf::Vector2f& pos);
	void Process(float frameTime);

	void TakeDamage(const int amount);

	void SetFacing(FacingDirection dir)
	{
		m_facing = dir;
	}

	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}

	float GetMovementSpeed()
	{
		return m_movement_speed;
	}

private:
	std::unique_ptr<sf::RectangleShape> m_shape;
	float m_movement_speed;
	sf::Vector2f m_last_velocity;
	int m_health;
	FacingDirection m_facing;
};