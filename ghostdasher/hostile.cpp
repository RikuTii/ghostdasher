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



void Hostile::GoToPosition(const sf::Vector2f& pos)
{
	static int last_dir = 0;
	if (m_move_delay < 0.1f)
	{

		if (m_path.size())
		{
			if (m_current_path_index < m_path.size())
			{
				sf::Vector2f dist = (m_position  - m_path.at(m_current_path_index));
				float dist_len = std::abs(std::sqrt(dist.x * dist.x + dist.y * dist.y));
		//		if(dist_len < 150.0f)
				m_position = m_path.at(m_current_path_index);
				m_current_path_index += 1;
			}
		}

		m_move_delay = 0.3f;
	}



	if (m_direction_time < 0.1f)
	{
		m_path = pathFinder->GenerateBestPath(m_position, pos);
		m_current_path_index = 0;
		m_direction_time = 100.5f;
	}
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


	m_move_delay -= frameTime * 100.0f;
	m_direction_time -= frameTime * 100.0f;

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
