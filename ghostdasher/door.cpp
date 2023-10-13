#include "door.h"


void Door::Init()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_shape->setSize(sf::Vector2f(40, 115));
	m_shape->setOrigin(sf::Vector2f(20, 20));
	m_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color::Blue);
	m_render_state = RenderState::Draw;
	m_type = EntityType::DoorEntity;
	m_is_open = false;
	m_open_timer = 0.0f;
}

Door::Door()
{
	Init();
}

Door::Door(const sf::Vector2f& pos)
{
	Init();

	m_position = pos;
}

void Door::SetOpen()
{
	if (!m_is_open)
	{
		m_open_timer = 0.0f;
	}
	m_is_open = true;
}

void Door::Process(float frameTime)
{
	m_shape->setPosition(m_position);

	if (m_is_open && m_open_timer < 15.0f)
	{
		m_position -= sf::Vector2f(0, (m_open_timer));
		m_open_timer += frameTime * 100.0f;
	}
}

void Door::Render(sf::RenderWindow& window)
{
	window.draw(*m_shape.get());
}