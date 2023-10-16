#include "crate.h"


Crate::Crate()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_shape->setSize(sf::Vector2f(40, 40));
	m_shape->setOrigin(sf::Vector2f(20, 20));
	m_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color::Yellow);
	m_render_state = RenderState::Draw;
	m_type = EntityType::CrateEntity;

	m_category = EntityCategory::CategoryGeneral;

}

void Crate::Process(float frameTime)
{
	m_shape->setPosition(m_position);
}

void Crate::Render(sf::RenderWindow& window)
{
	window.draw(*m_shape.get());
}