#include "Hostile.h"


Hostile::Hostile()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_shape->setSize(sf::Vector2f(40, 40));
	m_shape->setOrigin(sf::Vector2f(20, 20));

	m_position = sf::Vector2f((float)(rand() % 500), (float)(rand() % 500));
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color::Black);
	m_shape->setOutlineColor(sf::Color::Red);
	m_shape->setOutlineThickness(2);
	m_render_state = RenderState::Draw;
	m_movement_speed = 700.0f;
	m_type = EntityType::HostileEntity;
	m_health = 200;
}


void Hostile::TakeDamage(const int amount)
{
	m_health -= amount;
	if (m_health < 0)
	{
		Delete();
	}
}

void Hostile::Process(float frameTime)
{
	m_shape->setPosition(m_position);
}

void Hostile::Render(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw)
	{
		renderWindow.draw(*m_shape.get());

	}
}

void Hostile::SetPosition(const sf::Vector2f& pos)
{
	m_position = pos;
}
