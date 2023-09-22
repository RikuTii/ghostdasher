#include "entity.h"


Entity::Entity()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_shape->setSize(sf::Vector2f(40, 40));
	m_shape->setOrigin(sf::Vector2f(20, 20));
	m_position = sf::Vector2f((float)(rand() % 800), (float)(rand() % 800));
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color::White);
	m_render_state = RenderState::Draw;
	m_type = EntityType::GeneralEntity;
}



void Entity::Process(float frameTime)
{
	m_shape->setPosition(m_position);

	m_position = m_position + (m_velocity * frameTime);
	m_velocity = m_velocity + m_velocity_goal * frameTime;
}

void Entity::Render(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw)
	{
		renderWindow.draw(*m_shape.get());
	}
}

void Entity::SetPosition(const sf::Vector2f& pos)
{
	m_position = pos;
}

void Entity::SetVelocity(const sf::Vector2f& vel)
{
	m_velocity = vel;
}


void Entity::SetGoalVelocity(const sf::Vector2f& vel)
{
	m_velocity_goal = vel;
}