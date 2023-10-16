#include "firinghostile.h"

FiringHostile::FiringHostile()
{
	Init();

}

FiringHostile::FiringHostile(const sf::Vector2f& pos)
{
	Init();
	m_position = pos;
}

void FiringHostile::Init()
{
	m_shape = std::make_unique<sf::Sprite>();

	m_texture_sprite_size = sf::IntRect(0, 0, 24, 24);

	m_shape->setOrigin(sf::Vector2f(float(m_texture_sprite_size.width / 2), float(m_texture_sprite_size.height / 2)));

	m_position = sf::Vector2f((float)(rand() % 500), (float)(rand() % 500));
	m_goal_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
	m_render_state = RenderState::Draw;
	m_movement_speed = 700.0f;
	m_type = EntityType::HostileEntity;
	m_category = EntityCategory::CategoryGeneral;

	m_health = 200;
	LoadTextures();
	m_shape->setScale(3.0f, 3.0f);

	m_shape->setColor(sf::Color::Red);

	m_total_animation_frames = 4;

	m_spotted_text = std::make_unique<sf::Text>();

	m_spotted_text->setFillColor(sf::Color::Red);
	m_spotted_text->setCharacterSize(40);
	m_spotted_text->setFont(resourceManager->GetPrimaryFont());
	m_spotted_text->setString("!");
}
void FiringHostile::LoadTextures()
{

	std::optional<sf::Texture*> run_texture = resourceManager->GetTexture("HostileIdle");
	if (run_texture.has_value())
	{
		m_texture = run_texture.value();
		m_shape->setTexture(*m_texture);
	}

}

void FiringHostile::PlayAnimation(float frameTime)
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




void FiringHostile::Process(float frameTime)
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
		if (CanSeePlayer())
		{
			if (m_fire_time <= 0.0f)
			{
				std::unique_ptr<Projectile> projectile = std::make_unique<Projectile>();
				projectile->m_projectile_texture = std::make_unique<sf::RectangleShape>();
				projectile->m_projectile_texture->setFillColor(sf::Color::Magenta);
				projectile->m_projectile_texture->setSize(sf::Vector2f(5, 30));
				projectile->m_lifetime = 400.0f;
				projectile->m_life = projectile->m_lifetime;
				projectile->m_current_position = m_position;
				projectile->m_should_update = true;
				projectile->m_should_collide = true;
				sf::Vector2f target_end = m_target_position  + (m_target_position - m_position) * 10.0f;
				projectile->m_target_position = target_end;

				m_projectiles.push_back(std::move(projectile));

				m_fire_time = 200.0f;
			}


			if (!m_path_finding && m_spot_state != Spotted)
			{
				m_spotted_time = 15.0f;
				m_spot_state = Spotted;
				m_spotted_text->setString("!");
				m_spotted_text->setFillColor(sf::Color::Red);

			}

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

		DoRandomMovement(frameTime);
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

	for (auto const& it : m_projectiles)
	{

		it->m_life -= frameTime * 100.0f;

		if (it->m_should_update)
		{
			float angle = atan2f(it->m_target_position.x - it->m_current_position.x, it->m_target_position.y - it->m_current_position.y) * 180 / M_PI;
			it->m_projectile_texture->setRotation(-angle);

			sf::Vector2f delta = (it->m_target_position - it->m_current_position);

			const float veclen = std::sqrt(delta.x * delta.x + delta.y * delta.y);
			if (veclen > 0)
			{
				sf::Vector2f end_position = it->m_current_position + sf::Vector2f(delta.x, delta.y) / (veclen * ((it->m_life / it->m_lifetime) / 4));
				it->m_projectile_texture->setPosition(end_position);
				it->m_current_position = end_position;

				//this will make projectiles hoam towards the player
				it->m_target_position += (localplayer->GetPosition() - it->m_current_position) * 10.0f;

			}

		}

		if (it->m_should_collide && it->m_should_update)
		{
			sf::FloatRect projectileBounds(it->m_current_position.x, it->m_current_position.y, 10, 10);
			if (world->DoesIntersectWall(projectileBounds))
			{
				it->m_should_update = false;
			}

			if (projectileBounds.intersects(localplayer->GetBounds()))
			{
				localplayer->TakeDamage(50, m_position, ProjectileDamage);
				it->m_life = 0.0f;
			}
		}

	}

	std::erase_if(m_projectiles,
		[=](auto const& ptr) { return ptr && ptr->m_life <= 0.0f; });

	if (m_spotted_time > 0.0f)
	{
		m_spotted_text->setPosition(sf::Vector2f(GetPosition().x, GetPosition().y - 80));
		m_spotted_time -= frameTime * 10.0f;
	}

	if (m_fire_time > 0.0f)
	{
		m_fire_time -= frameTime * 100.0f;
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


void FiringHostile::Render(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw)
	{
		for (auto const& it : m_projectiles)
		{
			renderWindow.draw(*it->m_projectile_texture.get());
		}
		if (m_spotted_time > 0.0f)
		{
			renderWindow.draw(*m_spotted_text.get());
		}
		renderWindow.draw(*m_shape.get());

	}
}