#include "crate.h"
#include "world.h"
#include "entitymanager.h"

void Crate::Init()
{
	m_shape = std::make_unique<sf::Sprite>();
	m_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
	m_render_state = RenderState::Draw;
	m_type = EntityType::CrateEntity;


	m_texture_sprite_size = sf::IntRect(0, 0, 16, 32);

//	m_shape->setOrigin(sf::Vector2f(float(m_texture_sprite_size.width / 2), float(m_texture_sprite_size.height / 2)));
	m_shape->setScale(3.0f, 3.0f);

	m_broken = false;

	m_break_speed = 0.0f;

	m_category = EntityCategory::CategoryGeneral;

	LoadTextures();
}

Crate::Crate()
{
	Init();
}


Crate::Crate(const sf::Vector2f& pos)
{
	Init();
	m_position = pos;
	m_goal_position = m_position;
}


void Crate::LoadTextures()
{

	std::optional<sf::Texture*> chest_texture = resourceManager->GetTexture("Chest");
	if (chest_texture.has_value())
	{
		m_chest_texture = chest_texture.value();
		m_shape->setTexture(*m_chest_texture);
		sf::IntRect texture_rect = m_texture_sprite_size;
		texture_rect.left = (texture_rect.width * 0);
		m_shape->setTextureRect(texture_rect);
	}

	std::optional<sf::Texture*> potion_texture = resourceManager->GetTexture("Potion");
	if (potion_texture.has_value())
	{
		m_potion_texture = potion_texture.value();
	}

}


void Crate::Break()
{
	if (m_broken == false)
	{
		float x = float(rand() % 200 + 100);
		float y = float(rand() % 200 + 100);

		m_goal_position = m_position + sf::Vector2f(rand() % 2 > 0 ? x : -x, rand() % 2 > 0 ? y : -y);
		m_shape->setTexture(*m_potion_texture);
		m_break_speed = 1000.0f;
		m_texture_sprite_size = sf::IntRect(0, 0, 16, 16);
		m_shape->setTextureRect(m_texture_sprite_size);

		m_break_angle = float(rand() % 180);
		if (rand() % 2)
		{
			m_break_angle = -m_break_angle;
		}


		m_break_angle = m_break_angle * (M_PI / 180.0f);

		sf::Vector2f end = sf::Vector2f(m_position.x + (std::cos(m_break_angle)), m_position.y + std::sin(m_break_angle));

		sf::Vector2f delta = (m_position - end);

		if (rand() % 2)
			m_goal_position = m_position + (delta * 20.0f);
		else
			m_goal_position = m_position - (delta * 20.0f);


		m_broken = true;
	}
}

void Crate::Process(float frameTime)
{
	m_shape->setPosition(m_position);

	if (std::abs(m_position.x - m_goal_position.x) < 15.0f || std::abs(m_position.y - m_goal_position.y) < 15.0f)
	{
		World* world = entityManager->GetWorld();

		sf::Vector2f end_position = sf::Vector2f(m_position.x + (std::cos(m_break_angle) * m_break_speed * frameTime), m_position.y + std::sin(m_break_angle) * m_break_speed * frameTime);
		if (world->DoesIntersectWall(GetBounds()))
		{

		}
		else
		{
			m_position = end_position;
		}
	}
	

}

void Crate::Render(sf::RenderWindow& window)
{
	window.draw(*m_shape.get());
}