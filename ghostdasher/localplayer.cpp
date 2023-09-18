#include "localplayer.h"


LocalPlayer::LocalPlayer()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_shape->setSize(sf::Vector2f(40, 40));
	m_shape->setOrigin(sf::Vector2f(20, 20));

	m_sword = std::make_unique<sf::RectangleShape>();
	m_sword->setSize(sf::Vector2f(10, 45));
	m_sword->setOrigin(sf::Vector2f(0, 45));
	m_sword->setPosition(m_position);
	m_sword->setFillColor(sf::Color::Magenta);

	m_position = sf::Vector2f((float)(rand() % 500), (float)(rand() % 500));
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color::White);
	m_render_state = RenderState::Draw;
	m_movement_speed = 700.0f;
	m_attack_stage = 0.0f;
}


bool LocalPlayer::CheckSwordCollision(const sf::FloatRect& target)
{
	if (m_attack_stage > 0.0f && m_sword->getGlobalBounds().intersects(target))
	{
		return true;
	}

	return false;
}

void LocalPlayer::Process(float frameTime)
{
	m_shape->setPosition(m_position);
	m_sword->setPosition(m_position);


	if (m_facing == Right)
	{
		m_sword->move(sf::Vector2f(m_shape->getSize().x / 2 - 5, 0.0f));
		m_attack_angle = 180.0f;
	}
	if (m_facing == Left)
	{
		m_sword->move(sf::Vector2f(-m_shape->getSize().x / 2 + 5, 0.0f));
		m_attack_angle = 0;
	}

	if (m_facing == Down)
	{
		m_sword->move(sf::Vector2f(0, m_shape->getSize().y / 2 - 5));
		m_attack_angle = -90;
	}
	if (m_facing == Up)
	{
		m_sword->move(sf::Vector2f(0, -m_shape->getSize().y / 2 + 5));
		m_attack_angle = 90;
	}


	m_position = m_position + (m_velocity * frameTime);
	m_velocity = m_velocity + m_velocity_goal * frameTime;


	const float edgeSpeedScale = 5;

	if (m_position.x < 0.0f)
	{
		m_position.x = 0.0f;
		m_velocity.x = edgeSpeedScale;
		m_velocity_goal.x = edgeSpeedScale;
	}
	if (m_position.x > 1000.0f)
	{
		m_position.x = 1000.0f;
		m_velocity.x = -edgeSpeedScale;
		m_velocity_goal.x = -edgeSpeedScale;

	}

	if (m_position.y < 0.0f)
	{
		m_position.y = 0.0f;

		m_velocity.y = edgeSpeedScale;
		m_velocity_goal.y = edgeSpeedScale;

	}
	if (m_position.y > 1000.0f)
	{
		m_position.y = 1000.0f;
		m_velocity.y = -edgeSpeedScale;
		m_velocity_goal.y = -edgeSpeedScale;

	}

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
	m_sword->setRotation(m_attack_angle - (m_attack_stage * 45.0f));


	const float friction = 0.5f;

	if (m_velocity_goal.x > 10.0f || m_velocity_goal.x < -10.1f)
	{
		m_last_velocity = m_velocity_goal;
	}

	if (m_velocity_goal.x > -0.1f && m_velocity_goal.x < 0.1f)
	{
		if (m_velocity.x > 0.0f)
		{
			m_velocity.x = std::fmax(m_velocity.x - friction, 0.0f);
		}
		else
		{
			m_velocity.x = std::fmin(m_velocity.x + friction, 0.0f);
		}
	}
	if (m_velocity_goal.y > -0.1f && m_velocity_goal.y < 0.1f)
	{
		if (m_velocity.y > 0.0f)
		{
			m_velocity.y = std::fmax(m_velocity.y - friction, 0.0f);
		}
		else
		{
			m_velocity.y = std::fmin(m_velocity.y + friction, 0.0f);
		}
	}


	if (m_attack_stage > 0.0f)
	{
		m_attack_stage -= frameTime * 15.0f;
	}

}

void LocalPlayer::Render(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw)
	{
		if (m_attack_stage > 0.0f)
			renderWindow.draw(*m_sword.get());
		renderWindow.draw(*m_shape.get());

	}
}

void LocalPlayer::SetPosition(const sf::Vector2f& pos)
{
	m_position = pos;
}

void LocalPlayer::SetVelocity(const sf::Vector2f& vel)
{
	m_velocity_goal = vel;
}