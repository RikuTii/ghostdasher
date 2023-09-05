#include "entity.h"


Entity::Entity()
{
	m_shape = std::make_unique<sf::RectangleShape>();
//	m_shape->setRadius(rand() % 100);
	m_shape->setSize(sf::Vector2f(40, 40));
	m_shape->setOrigin(sf::Vector2f(20, 20));
	m_position = sf::Vector2f(rand() % 500, rand() % 500);
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color::White);
}



void Entity::Process(float frameTime)
{
	m_shape->setPosition(m_position);

	m_position = m_position + (m_velocity * frameTime);
	m_velocity = m_velocity + m_velocityGoal * frameTime;

	m_shape->setRotation(m_velocity.x * 0.3f);

	const float friction = 0.5f;


	if (m_velocityGoal.x > -0.1f && m_velocityGoal.x < 0.1f)
	{
		if (m_velocity.x > 0.0f)
		{
			m_velocity.x -= friction;
		}
		else
		{
			m_velocity.x += friction;
		}
	}
	if (m_velocityGoal.y > -0.1f && m_velocityGoal.y < 0.1f)
	{
		if (m_velocity.y > 0.0f)
		{
			m_velocity.y -= friction;
		}
		else
		{
			m_velocity.y += friction;
		}
	}

}

void Entity::Render(sf::RenderWindow& renderWindow)
{
	renderWindow.draw(*m_shape.get());
	
}

void Entity::SetPosition(const sf::Vector2f& pos)
{
	m_position = pos;
}

void Entity::SetVelocity(const sf::Vector2f& vel)
{
	m_velocityGoal = vel;
}