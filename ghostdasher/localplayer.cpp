#include "localplayer.h"
#include "entitymanager.h"


LocalPlayer::LocalPlayer()
{
	m_shape = std::make_unique<sf::Sprite>();

	m_texture_sprite_size = sf::IntRect(0, 0, 64, 64);

	m_shape->setOrigin(sf::Vector2f(float(m_texture_sprite_size.width / 2), float(m_texture_sprite_size.height / 2)));

	m_sword = std::make_unique<sf::RectangleShape>();
	m_sword->setSize(sf::Vector2f(10, 100));
	m_sword->setOrigin(sf::Vector2f(0, 100));
	m_sword->setPosition(m_position);
	m_sword->setOutlineColor(sf::Color::Magenta);
	m_sword->setOutlineThickness(2);

	m_shape->setPosition(m_position);

	m_sprite_scale = sf::Vector2f(1.2f, 1.2f);

	m_shape->setScale(m_sprite_scale);


	m_render_state = RenderState::Draw;
	m_movement_speed = 500.0f;
	m_attack_stage = 0.0f;

	m_total_animation_frames = 3;
	m_animation_frame = 0;

	m_health = 200;


	LoadTextures();
}


void LocalPlayer::LoadTextures()
{

	std::optional<sf::Texture*> run_texture = resourceManager->GetTexture("LocalRun");
	if (run_texture.has_value())
	{
		m_texture = run_texture.value();
	}

	std::optional<sf::Texture*> idle_texture = resourceManager->GetTexture("LocalIdle");
	if (idle_texture.has_value())
	{
		m_idle_texture = idle_texture.value();
	}

}


void LocalPlayer::PlayAnimation(float frameTime)
{
	if (m_animation_time < 0.0f)
	{
		m_animation_frame++;

		if (m_animation_frame >= m_total_animation_frames)
			m_animation_frame = 0;

		sf::IntRect texture_rect = m_texture_sprite_size;
		texture_rect.left = (texture_rect.width * m_animation_frame);

		if (m_current_animation != Idle)
		{
			if (m_facing == Up)
			{
				texture_rect.top = (texture_rect.height * 1);

			}
			else if (m_facing == Left || m_facing == Right)
			{
				texture_rect.top = (texture_rect.height * 2);

			}
			else
			{
				texture_rect.top = (texture_rect.height * 0);

			}
		}
		else
		{
			if (m_animation_frame < 3)
			{
				texture_rect.left = (texture_rect.width * m_animation_frame);
				texture_rect.top = (texture_rect.height * 0);
			}
			else if (m_animation_frame < 6)
			{
				texture_rect.left = (texture_rect.width * (m_animation_frame - 3));
				texture_rect.top = (texture_rect.height * 1);
			}
			else if (m_animation_frame < 9)
			{
				texture_rect.left = (texture_rect.width * (m_animation_frame - 6));
				texture_rect.top = (texture_rect.height * 2);
			}
			else
			{
				texture_rect.left = (texture_rect.width * (m_animation_frame - 9));
				texture_rect.top = (texture_rect.height * 3);
			}
		}
		m_shape->setTextureRect(texture_rect);
	//	m_shape->setTexture(*m_texture);
		m_animation_time = 12.0f;
	}

	m_animation_time -= frameTime * 100.0f;
}

void LocalPlayer::DoDash()
{
	if(m_dash_time <= 0.0f)
	m_dash_time = 10.1f;
}


void LocalPlayer::DoKnockbackMove()
{
	const float knockbackScale = 5.0f;
	if (m_position.x < m_goal_position.x)
	{
		m_position.x += knockbackScale;
	}
	else if (m_position.x > m_goal_position.x)
	{
		m_position.x -= knockbackScale;
	}
	else if (m_position.y < m_goal_position.y)
	{
		m_position.y += knockbackScale;
	}
	else if (m_position.y > m_goal_position.y)
	{
		m_position.y -= knockbackScale;
	}
	else
	{
		m_position = m_goal_position;
	}
}

void LocalPlayer::TakeDamage(const int damage, const sf::Vector2f& pos, DamageType type)
{
	if ((globals->tick_count - m_last_damage_tick) > 128)
	{
		m_health -= damage;
		m_knockback_time = 30.0f;

		if (type == MeleeDamage)
		{

			World* world = entityManager->GetWorld();

			const float moveDistance = 150.0f;
			FacingDirection dir = FacingDirection::Down;

			float angle = atan2f(pos.x - m_position.x, pos.y - m_position.y) * 180 / 3.14f;
			if (angle > 0.0f && angle < 90.0f)
			{
				dir = FacingDirection::Left;
			}
			else if (angle >= 90.0f && angle < 180.0f)
			{
				dir = FacingDirection::Down;
			}
			else if (angle < 0.0f && angle > -90.0f)
			{
				dir = FacingDirection::Right;
			}
			else if (angle <= -90.0f && angle > -180.0f)
			{
				dir = FacingDirection::Up;
			}
			if (dir == FacingDirection::Right)
			{
				m_goal_position = m_position + sf::Vector2f(moveDistance, 0);
				int intersection = world->GetIntersection(m_goal_position);
				if (intersection & IntersectionDirection::InteresecRight)
				{
					while (true)
					{
						m_goal_position.x -= 5.0f;
						intersection = world->GetIntersection(m_goal_position);
						if (m_goal_position.x < m_position.x || !(intersection & IntersectionDirection::InteresecRight))
							break;
					}
				}

			}
			else if (dir == FacingDirection::Left)
			{
				m_goal_position = m_position - sf::Vector2f(moveDistance, 0);
				int intersection = world->GetIntersection(m_goal_position);
				if (intersection & IntersectionDirection::InteresecLeft)
				{
					while (true)
					{
						m_goal_position.x += 5.0f;
						intersection = world->GetIntersection(m_goal_position);
						if (m_goal_position.x > m_position.x || !(intersection & IntersectionDirection::InteresecLeft))
							break;
					}
				}
			}
			else if (dir == FacingDirection::Down)
			{
				m_goal_position = m_position + sf::Vector2f(0, moveDistance);
				int intersection = world->GetIntersection(m_goal_position);
				if (intersection & IntersectionDirection::InteresecDown)
				{
					while (true)
					{
						m_goal_position.y -= 5.0f;
						intersection = world->GetIntersection(m_goal_position);
						if (m_goal_position.y < m_position.y || !(intersection & IntersectionDirection::InteresecDown))
							break;
					}
				}
			}
			else if (dir == FacingDirection::Up)
			{
				m_goal_position = m_position - sf::Vector2f(0, moveDistance);
				int intersection = world->GetIntersection(m_goal_position);
				if (intersection & IntersectionDirection::InteresecUp)
				{
					while (true)
					{
						m_goal_position.y += 5.0f;
						intersection = world->GetIntersection(m_goal_position);
						if (m_goal_position.y > m_position.y || !(intersection & IntersectionDirection::InteresecUp))
							break;
					}
				}
			}
		}
		else
		{
			m_goal_position = m_position;
		}


		m_last_damage_tick = globals->tick_count;
	}
}

bool LocalPlayer::CheckSwordCollision(const sf::FloatRect& target)
{
	if (m_attack_stage > 0.0f && m_sword->getGlobalBounds().intersects(target) && globals->tick_count > m_last_attack_tick)
	{
		m_last_attack_tick = INT_MAX;
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
		m_shape->setScale(sf::Vector2f(-m_sprite_scale.x, m_sprite_scale.y));

	}
	if (m_facing == Left)
	{
		m_shape->setScale(sf::Vector2f(m_sprite_scale.x, m_sprite_scale.y));
	}


	if (m_velocity_goal.x > 10.1f || m_velocity_goal.x < -10.1f || (m_velocity_goal.y > 10.1f || m_velocity_goal.y < -10.1f) || m_attack_stage > 0.0f)
	{
		m_total_animation_frames = 4;
		m_shape->setTexture(*m_texture);
		m_current_animation = Run;
	}
	else
	{
		m_total_animation_frames = 12;
		m_shape->setTexture(*m_idle_texture);
		m_current_animation = Idle;
	}

	if (m_facing == Right)
	{
		m_sword->move(sf::Vector2f(float(m_texture_sprite_size.width / 4 - 5), 0.0f));
		m_attack_angle = 180.0f;
	}
	if (m_facing == Left)
	{
		m_sword->move(sf::Vector2f(float(-m_texture_sprite_size.width / 4 + 5), 0.0f));
		m_attack_angle = 0;
	}

	if (m_facing == Down)
	{
		m_sword->move(sf::Vector2f(0, float(m_texture_sprite_size.height / 4 - 5)));
		m_attack_angle = -90;
	}
	if (m_facing == Up)
	{
		m_sword->move(sf::Vector2f(0, float(-m_texture_sprite_size.height / 4 + 5)));
		m_attack_angle = 90;
	}

	if (m_knockback_time > 0.0f)
	{
		DoKnockbackMove();
	}
	else
	{
		ProcessMovement(frameTime);
	}

	m_sword->setRotation(m_attack_angle - (m_attack_stage * 45.0f));


	if (m_attack_stage > 0.0f)
	{
		m_attack_stage -= frameTime * 35.0f;
	}
	else
	{
		m_last_attack_tick = 0;
	}


	if (m_current_animation != m_last_animation)
	{
		m_animation_frame = 0;
		m_animation_time = -0.1f;
	}

	PlayAnimation(frameTime);

	if (m_knockback_time > 0.0f)
	{
		m_knockback_time -= frameTime * 100.0f;
	}

	m_last_animation = m_current_animation;

}


void LocalPlayer::ProcessMovement(float frameTime)
{
	if (m_dash_time > 0.0f)
	{
		sf::Vector2f cur_goal = m_velocity_goal;
		m_velocity_goal *= 10.0f;
		m_dash_time -= (frameTime * 100.0f);

		if (m_dash_time < 0.0f)
		{
			m_velocity = cur_goal;
		}
	}

	if (m_velocity_goal.x > 0.0f && m_velocity.x < 0.0f || m_velocity_goal.x < 0.0f && m_velocity.x > 0.0f)
	{
		m_velocity.x = 0.0f;
	}
	if (m_velocity_goal.y > 0.0f && m_velocity.y < 0.0f || m_velocity_goal.y < 0.0f && m_velocity.y > 0.0f)
	{
		m_velocity.y = 0.0f;
	}

	m_velocity = m_velocity_goal;

	World* world = entityManager->GetWorld();


	m_position = m_position + (m_velocity * frameTime);

	world->TryMovement(this);


	m_sword->setRotation(m_attack_angle - (m_attack_stage * 45.0f));


	const float friction = 0.8f;

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
