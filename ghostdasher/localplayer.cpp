#include "localplayer.h"
#include "entitymanager.h"


LocalPlayer::LocalPlayer()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_shape->setSize(sf::Vector2f(40, 40));
	m_shape->setOrigin(sf::Vector2f(20, 20));

	m_sword = std::make_unique<sf::RectangleShape>();
	m_sword->setSize(sf::Vector2f(10, 100));
	m_sword->setOrigin(sf::Vector2f(0, 100));
	m_sword->setPosition(m_position);
	m_sword->setOutlineColor(sf::Color::Magenta);
	m_sword->setOutlineThickness(2);

	m_position = sf::Vector2f((float)(rand() % 500), (float)(rand() % 500));
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color::Black);
	m_shape->setOutlineColor(sf::Color::Blue);
	m_shape->setOutlineThickness(2);
	m_render_state = RenderState::Draw;
	m_movement_speed = 700.0f;
	m_attack_stage = 0.0f;
}


void LocalPlayer::DoDash()
{
	m_dash_time = 5.0f;
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

	if (m_dash_time > 0.0f)
	{
		sf::Vector2f cur_goal = m_velocity_goal;
		m_velocity_goal *= 50.0f;
		m_dash_time -= (frameTime * 100.0f);

		if (m_dash_time < 0.0f)
		{
			m_velocity = cur_goal;
		}
	}



	m_velocity = m_velocity + m_velocity_goal * frameTime;


	World* world = m_entityManager->GetWorld();

	world->TryMovement(this);

	m_position = m_position + (m_velocity * frameTime);


	m_shape->setRotation(m_velocity.x * 0.3f);
	m_sword->setRotation(m_attack_angle - (m_attack_stage * 45.0f));


	const float friction = 0.99f;

	if (m_velocity_goal.x > 10.0f || m_velocity_goal.x < -10.1f)
	{
		m_last_velocity = m_velocity_goal;
	}

	if (m_velocity_goal.x > -0.1f && m_velocity_goal.x < 0.1f)
	{
		if (m_velocity.x > 0.0f)
		{
			m_velocity.x = std::fmax(m_velocity.x * friction, 0.0f);
		}
		else
		{
			m_velocity.x = std::fmin(m_velocity.x * friction, 0.0f);
		}
	}
	if (m_velocity_goal.y > -0.1f && m_velocity_goal.y < 0.1f)
	{
		if (m_velocity.y > 0.0f)
		{
			m_velocity.y = std::fmax(m_velocity.y * friction, 0.0f);
		}
		else
		{
			m_velocity.y = std::fmin(m_velocity.y * friction, 0.0f);
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
