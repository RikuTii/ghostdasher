#include "ScaredHostile.h"

ScaredHostile::ScaredHostile()
{
	Init();

}

ScaredHostile::ScaredHostile(const sf::Vector2f& pos)
{
	Init();
	m_position = pos;
}

void ScaredHostile::Init()
{
	m_shape = std::make_unique<sf::Sprite>();
	m_texture_sprite_size = sf::IntRect(0, 0, 24, 24);
	m_shape->setOrigin(sf::Vector2f(float(m_texture_sprite_size.width / 2), float(m_texture_sprite_size.height / 2)));
	m_position = sf::Vector2f((float)(rand() % 500), (float)(rand() % 500));
	m_goal_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
	m_render_state = RenderState::Draw;
	m_movement_speed = 700.0f;
	m_type = EntityType::ScaredHostileEntity;
	m_category = EntityCategory::CategoryGeneral;
	m_health = 200;
	LoadTextures();
	m_shape->setScale(3.0f, 3.0f);

	m_shape->setColor(sf::Color::Yellow);

	m_total_animation_frames = 4;

	m_spotted_text = std::make_unique<sf::Text>();

	m_spotted_text->setFillColor(sf::Color::Red);
	m_spotted_text->setCharacterSize(40);
	m_spotted_text->setFont(resourceManager->GetPrimaryFont());
	m_spotted_text->setString("!");

	m_retreat_time = 0.0f;
}
void ScaredHostile::LoadTextures()
{

	std::optional<sf::Texture*> run_texture = resourceManager->GetTexture("HostileIdle");
	if (run_texture.has_value())
	{
		m_texture = run_texture.value();
		m_shape->setTexture(*m_texture);
	}

}

void ScaredHostile::PlayAnimation(float frameTime)
{
	if (m_animation_time < 0.0f)
	{
		m_animation_frame++;

		if (m_animation_frame >= (m_min_animation_frame + m_total_animation_frames))
			m_animation_frame = m_min_animation_frame;

		sf::IntRect texture_rect = m_texture_sprite_size;
		texture_rect.left = (texture_rect.width * m_animation_frame);
		m_shape->setTextureRect(texture_rect);
		m_animation_time = 8.0f;
	}

	m_animation_time -= frameTime * 100.0f;
}

void ScaredHostile::TakeDamage(const int amount, FacingDirection dir)
{
	if ((globals->tick_count - m_last_damage_tick) > 50)
	{
		m_health -= amount;
		if (m_health <= 0)
		{
			Delete();
		}

		m_knockback_time = 5.0f;

		m_last_damage_tick = globals->tick_count;

		CalculateKnockback(dir);

		m_retreat_time = 1350.0f;


		World* world = entityManager->GetWorld();

		m_target_position = world->GetFurthestSpawnpoint(m_position);
	}
	
}



void ScaredHostile::Process(float frameTime)
{
	m_shape->setPosition(m_position);


	if (m_goal_position.x < m_position.x)
	{
		m_facing = Right;
	}
	else if (m_goal_position.x > m_position.x)
	{
		m_facing = Left;
	}

	else if (m_goal_position.y < m_position.y)
	{
		m_facing = Up;
	}
	else if (m_goal_position.y > m_position.y)
	{
		m_facing = Down;
	}



	if (m_goal_position.x < 0.1f && m_goal_position.x > -0.1f)
		m_goal_position = m_position;



	if (m_knockback_time > 0.0f)
	{
		DoKnockbackMove();
	}
	else
	{
		if(m_retreat_time <= 0.0f)
		if (CanSeePlayer())
		{
			if (!m_path_finding)
			{
				m_spotted_time = 15.0f;
				m_spot_state = Spotted;
				m_spotted_text->setString("!");
				m_spotted_text->setFillColor(sf::Color::Red);

			}

			m_path_finding = true;
			
		}
		else if ((globals->curtime - m_last_spotted_player) > 5.0f)
		{
			if (m_spot_state == Spotted)
			{
				m_spotted_time = 15.0f;
				m_spot_state = Lost;
				m_spotted_text->setString("?");
				m_spotted_text->setFillColor(sf::Color::White);

			}
			m_path_finding = false;
		}

		if(!m_path_finding)
			DoRandomMovement(frameTime);
	}

	if (m_path_finding)
	{
		GoToPosition();
	}


	if (m_facing == Right)
	{
		m_shape->setScale(sf::Vector2f(3.0f, 3.0f));
		m_min_animation_frame = 4;

	}
	else if (m_facing == Left)
	{
		m_shape->setScale(sf::Vector2f(-3.0f, 3.0f));
		m_min_animation_frame = 4;
	}
	else if (m_facing == Up)
	{
		m_shape->setScale(sf::Vector2f(3.0f, 3.0f));
		m_min_animation_frame = 12;
	}
	else
	{
		m_min_animation_frame = 0;
	}

	World* world = entityManager->GetWorld();
	LocalPlayer* localplayer = entityManager->GetLocalPlayer();


	if (m_spotted_time > 0.0f)
	{
		m_spotted_text->setPosition(sf::Vector2f(GetPosition().x, GetPosition().y - 80));
		m_spotted_time -= frameTime * 10.0f;
	}

	if (m_retreat_time > 0.0f)
	{
		m_retreat_time -= frameTime * 100.0f;
	}


	if (m_knockback_time > 0.0f)
	{
		m_knockback_time -= frameTime * 100.0f;
	}
	if (m_move_delay > 0.0f)
	{
		m_move_delay -= frameTime * 100.0f;
	}
	if (m_direction_time > 0.0f)
	{
		m_direction_time -= frameTime * 100.0f;
	}

	PlayAnimation(frameTime);

	m_last_position = m_position;

}


void ScaredHostile::Render(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw)
	{
		if (m_spotted_time > 0.0f)
		{
			renderWindow.draw(*m_spotted_text.get());
		}
		renderWindow.draw(*m_shape.get());
	}
}