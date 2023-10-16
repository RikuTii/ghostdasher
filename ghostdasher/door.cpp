#include "door.h"
#include "bouncingboss.h"

void Door::Init()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_shape->setSize(sf::Vector2f(115, 40));
	m_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color::Blue);
	m_render_state = RenderState::Draw;
	m_type = EntityType::DoorEntity;
	m_is_open = false;
	m_opening = false;
	m_closing = false;

	m_open_speed = 400.0f;

	m_category = EntityCategory::CategoryBoss;


	m_origin_position = m_position;
}

Door::Door()
{
	Init();
}

Door::Door(const sf::Vector2f& pos, const sf::Vector2f& size)
{
	Init();

	m_position = pos;
	m_origin_position = m_position;
	m_shape->setSize(size);
}

void Door::SetOpen()
{
	if (!m_is_open)
	{
		m_opening = true;
	}
	m_is_open = true;
}

void Door::Process(float frameTime)
{
	m_shape->setPosition(m_position);

	if (m_is_open && m_opening)
	{
		if (m_position.x < m_origin_position.x + GetBounds().width)
		{
			m_position.x += m_open_speed * frameTime;
		}
		else
		{
			m_opening = false;
			m_closing = false;
			m_active_trigger = true;
		}
	}

	if (m_active_trigger && !m_closing)
	{
		LocalPlayer* player = entityManager->GetLocalPlayer();
		World* world = entityManager->GetWorld();
		if (player->GetBounds().intersects(m_trigger_position))
		{
			entityManager->AddEntity(std::make_unique<BouncingBoss>(world->GetBossPosition()));
			m_closing = true;
			world->TriggerBossfight();
			m_active_trigger = false;
		}
	}

	if (m_closing)
	{
		if (m_closing && m_position.x > m_origin_position.x)
		{
			m_position.x -= m_open_speed * frameTime;
		}
		else
		{
			m_closing = false;
			m_opening = false;
		}
	}
}

void Door::Render(sf::RenderWindow& window)
{
	window.draw(*m_shape.get());
}