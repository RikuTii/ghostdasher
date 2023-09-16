#include "entity.h"


Entity::Entity()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_shape->setSize(sf::Vector2f(40, 40));
	m_shape->setOrigin(sf::Vector2f(20, 20));
	m_position = sf::Vector2f(rand() % 500, rand() % 500);
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color::White);
	m_render_state = RenderState::Draw;
}



void Entity::Process(float frameTime)
{
	m_shape->setPosition(m_position);

	m_position = m_position + (m_velocity * frameTime);
	m_velocity = m_velocity + m_velocityGoal * frameTime;


	const float edgeSpeedScale = 5;

	if (m_position.x < 0.0f)
	{
		m_velocity.x = edgeSpeedScale;
	}
	if (m_position.x > 1000.0f)
	{
		m_velocity.x = -edgeSpeedScale;
	}

	if (m_position.y < 0.0f)
	{
		m_velocity.y = edgeSpeedScale;
	}
	if (m_position.y > 1000.0f)
	{
		m_velocity.y = -edgeSpeedScale;
	}

	//obstacle.setSize(sf::Vector2f(300, 300));
	//obstacle.setPosition(sf::Vector2f(400, 400));
	const float velModifierBounce = 200;


	struct UnwalkableSpace
	{
		sf::Vector2f m_pos;
		sf::Vector2f m_size;
	};

	std::vector<UnwalkableSpace> m_spaces;

	{
		UnwalkableSpace limt;
		limt.m_pos = sf::Vector2f(400, 400);
		limt.m_size = sf::Vector2f(300, 300);
		m_spaces.push_back(limt);
	}

	{
		UnwalkableSpace limt;
		limt.m_pos = sf::Vector2f(200, 200);
		limt.m_size = sf::Vector2f(150, 300);
		m_spaces.push_back(limt);
	}


	{
		UnwalkableSpace limt;
		limt.m_pos = sf::Vector2f(200, 600);
		limt.m_size = sf::Vector2f(300, 80);
		m_spaces.push_back(limt);
	}


	const float scaleError = m_shape.get()->getGlobalBounds().height;

	for (auto it : m_spaces)
	{
		float x = it.m_pos.x;
		float y = it.m_pos.y;
		float w = it.m_pos.x + it.m_size.x;
		float h = it.m_pos.y + it.m_size.y;

		if (m_position.x > x && m_position.x < w && (m_position.y < h && m_position.y > y))
		{

			if (m_position.x > x && m_position.x < w - scaleError)
			{
				m_velocity.x = -velModifierBounce;
			}

			else if (m_position.x > x + scaleError)
			{
				m_velocity.x = velModifierBounce;
			}

			if (m_position.y > y && m_position.y < h - scaleError)
			{
				m_velocity.y = -velModifierBounce;
			}

			else if (m_position.y > y + scaleError)
			{
				m_velocity.y = velModifierBounce;
			}
		}
	}



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
	m_velocityGoal = vel;
}