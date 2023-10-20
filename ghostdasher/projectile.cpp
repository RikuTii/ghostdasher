#include "projectile.h"

void Projectile::Init()
{
	m_shape = std::make_unique<sf::Sprite>();
	m_shape->setScale(4.0f,4.0f);
	m_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);


	m_texture_sprite_size = sf::IntRect(0, 0, 3, 15);

	m_shape->setOrigin(sf::Vector2f(float(m_texture_sprite_size.width / 2), float(m_texture_sprite_size.height / 2)));

	m_render_state = RenderState::Draw;
	m_type = EntityType::ProjectileEntity;
	m_category = EntityCategory::CategoryGeneral;

	LoadTextures();
	m_total_animation_frames = 0;

	m_lifetime = 400.0f;
	m_should_collide = false;
	m_should_update = true;
	m_life = m_lifetime;
	m_should_explode = false;
	m_exploded = false;

}

Projectile::Projectile()
{
	Init();
}

Projectile::Projectile(const sf::Vector2f& pos, const sf::Vector2f& target)
{
	Init();
	m_position = pos;
	m_target_position = target;
	m_should_collide = true;
	sf::Vector2f target_end = m_target_position + (m_target_position - m_position) * 100.0f;
	m_target_position = target_end;
	m_should_explode = true;
}


void Projectile::LoadTextures()
{

	std::optional<sf::Texture*> projectile_texture = resourceManager->GetTexture("Projectile");
	if (projectile_texture.has_value())
	{
		m_projectile_texture = projectile_texture.value();
		m_shape->setTexture(*m_projectile_texture);
	}

	std::optional<sf::Texture*> projectile_exploded = resourceManager->GetTexture("ProjectileExplosion");
	if (projectile_exploded.has_value())
	{
		m_exploded_texture = projectile_exploded.value();
	}
}

void Projectile::PlayAnimation(float frameTime)
{
	if (m_animation_time < 0.0f && m_total_animation_frames)
	{
		m_animation_frame++;

		if (m_animation_frame >= (m_min_animation_frame + m_total_animation_frames))
			m_animation_frame = m_min_animation_frame;

		const int row_frames = 5;
		sf::IntRect texture_rect = m_texture_sprite_size;
		if (m_animation_frame < row_frames)
		{
			texture_rect.left = (texture_rect.width * m_animation_frame);
			texture_rect.top = (texture_rect.height * 0);
		}
		else if (m_animation_frame < (row_frames*2))
		{
			texture_rect.left = (texture_rect.width * (m_animation_frame - (row_frames)));
			texture_rect.top = (texture_rect.height * 1);
		}
		else if (m_animation_frame < (row_frames*3))
		{
			texture_rect.left = (texture_rect.width * (m_animation_frame - (row_frames * 2)));
			texture_rect.top = (texture_rect.height * 2);
		}
		else
		{
			texture_rect.left = (texture_rect.width * (m_animation_frame - (row_frames * 3)));
			texture_rect.top = (texture_rect.height * 3);
		}


		m_shape->setTextureRect(texture_rect);
		m_animation_time = 8.0f;
	}

	m_animation_time -= frameTime * 100.0f;
}



void Projectile::Explode()
{
	if (!m_exploded)
	{
		m_life = m_lifetime;
		m_exploded = true;
		m_should_update = false;

		m_total_animation_frames = 19;
		m_texture_sprite_size = sf::IntRect(0, 0, 98, 98);
		m_shape->setOrigin(sf::Vector2f(float(m_texture_sprite_size.width / 2), float(m_texture_sprite_size.height / 2)));
		m_shape->setTexture(*m_exploded_texture);
		m_shape->setRotation(0.0f);
		m_shape->setScale(2.0f, 2.0f);
		m_position.y -= 98.0f;
	}

}

void Projectile::Process(float frameTime)
{
	m_shape->setPosition(m_position);

	if (m_life > 0.0f)
	{
		m_life -= frameTime * 100.0f;
	}
	else
	{
		Delete();
	}

	LocalPlayer* localplayer = entityManager->GetLocalPlayer();

	World* world = entityManager->GetWorld();

	if (m_should_update)
	{
		float angle = atan2f(m_target_position.x - m_position.x, m_target_position.y - m_position.y) * 180 / M_PI;
		m_shape->setRotation(-angle);

		sf::Vector2f delta = (m_target_position - m_position);

		const float veclen = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		if (veclen > 0)
		{
			sf::Vector2f end_position = m_position + sf::Vector2f(delta.x, delta.y) / (veclen * ((m_life / m_lifetime) / 4));
			m_position = end_position;

			//this will make projectiles hoam towards the target
			if (m_should_hoam_target)
			{
				m_target_position += (localplayer->GetPosition() - m_position) * 10.0f;
			}

		}
	}

	sf::FloatRect projectileBounds(m_position.x, m_position.y, 10, 10);

	if (m_should_collide && m_should_update && !m_exploded)
	{
		if (world->DoesIntersectWall(projectileBounds))
		{
			m_should_update = false;
			if (m_should_explode)
			{
				Explode();
			}
		}

		if (m_target_entity_index > 0)
		{
			Entity* target_entity = entityManager->GetEntity(m_target_entity_index);
			if (target_entity)
			{
				if (target_entity->GetType() == LocalplayerEntity)
				{
					LocalPlayer* player = dynamic_cast<LocalPlayer*>(target_entity);
					if (player)
					{
						if (projectileBounds.intersects(player->GetBounds()))
						{
							player->TakeDamage(50, m_position, ProjectileDamage);
							m_life = 0.0f;
							Delete();
						}
					}
				}
			}
		}
		else
		{
			if (projectileBounds.intersects(localplayer->GetBounds()))
			{
				if (m_should_explode)
				{
					localplayer->TakeDamage(50, m_position, ProjectileDamage);
					Explode();
				}
				else
				{
					localplayer->TakeDamage(50, m_position, ProjectileDamage);
					m_life = 0.0f;
					Delete();
				}
			}
		}
	}
	else if (m_exploded)
	{
		if (projectileBounds.intersects(localplayer->GetBounds()))
		{
			localplayer->TakeDamage(50, m_position);
		}
	}


	PlayAnimation(frameTime);
}

void Projectile::Render(sf::RenderWindow& window)
{
	window.draw(*m_shape.get());
}