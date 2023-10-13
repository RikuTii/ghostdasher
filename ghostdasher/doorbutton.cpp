#include "doorbutton.h"


void DoorButton::Init()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_shape->setSize(sf::Vector2f(40, 40));
	m_shape->setOrigin(sf::Vector2f(20, 20));
	m_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color::Red);
	m_render_state = RenderState::Draw;
	m_type = EntityType::DoorButtonEntity;
	m_is_pressed = false;
}

DoorButton::DoorButton()
{
	Init();
}
DoorButton::DoorButton(const sf::Vector2f& pos)
{
	Init();

	m_position = pos;
}
void DoorButton::Process(float frameTime)
{
	m_shape->setPosition(m_position);

	m_shape->setFillColor(m_is_pressed ? sf::Color::Green : sf::Color::Red);
}

void DoorButton::Render(sf::RenderWindow& window)
{
	window.draw(*m_shape.get());
}