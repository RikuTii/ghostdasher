#include "localplayer.h"
#include "entitymanager.h"
#include "window.h"

LocalPlayer::LocalPlayer()
{
	m_shape = std::make_unique<sf::Sprite>();

	m_texture_sprite_size = sf::IntRect(0, 0, 64, 64);

	m_shape->setOrigin(sf::Vector2f(float(m_texture_sprite_size.width / 2), float(m_texture_sprite_size.height / 2)));

	m_sword = std::make_unique<sf::Sprite>();

	//m_sword->setOrigin(sf::Vector2f(0, 100));
	m_sword->setPosition(m_position);
	m_sword->setOrigin(sf::Vector2f(0, 50));
	m_sword->setScale(3.0f, 3.0f);


	m_heart_shape = std::make_unique<sf::CircleShape>();;
	m_heart_shape->setOutlineColor(sf::Color::Cyan);
	m_heart_shape->setRadius(10.0f);
	m_heart_shape->setFillColor(sf::Color::Black);
	m_heart_shape->setOutlineThickness(2);

	m_damage_indicator = std::make_unique<sf::CircleShape>();
	m_damage_indicator->setOutlineColor(sf::Color::Red);
	m_damage_indicator->setRadius(50.0f);
	m_damage_indicator->setOutlineThickness(2);
	m_damage_indicator->setFillColor(sf::Color::Transparent);
	m_damage_indicator->setPosition(sf::Vector2f(window->getDefaultView().getSize().x / 2 - m_damage_indicator->getLocalBounds().width / 2, 
		window->getDefaultView().getSize().y / 2 - m_damage_indicator->getLocalBounds().height / 2));


	m_respawn_background = std::make_unique<sf::RectangleShape>();
	m_respawn_background->setSize(sf::Vector2f(float(window->getSize().x), float(window->getSize().y)));
	m_respawn_background->setFillColor(sf::Color::Transparent);

	m_death_text = std::make_unique<sf::Text>();
	m_death_text->setFillColor(sf::Color::Red);
	m_death_text->setCharacterSize(60);
	m_death_text->setFont(resourceManager->GetPrimaryFont());
	m_death_text->setString("YOU DIED");
	m_death_text->setPosition(window->getSize().x / 2 - m_death_text->getGlobalBounds().width / 2, window->getSize().y / 2 - m_death_text->getGlobalBounds().height / 2);

	m_shape->setPosition(m_position);

	m_sprite_scale = sf::Vector2f(1.2f, 1.2f);

	m_shape->setScale(m_sprite_scale);

	m_render_state = RenderState::Draw;
	m_type = EntityType::LocalplayerEntity;
	m_movement_speed = 500.0f;
	m_attack_stage = 0.0f;

	m_total_animation_frames = 3;
	m_animation_frame = 0;

	m_max_health = 400;

	m_health = m_max_health;

	LoadTextures();
}


void LocalPlayer::Respawn()
{
	m_health = m_max_health;
	m_attack_stage = 0.0f;
	m_death_time = 0.0f;
	m_attack_angle = 0.0f;
	m_dash_time = 0.0f;
	m_animation_time = 0.0f;
	m_last_attack_tick = 0;
	m_last_attack_time = 0.0f;
	m_last_damage_tick = 0;
	m_last_velocity = sf::Vector2f();
	m_damage_time = 0.0f;
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

	std::optional<sf::Texture*> sword_texture = resourceManager->GetTexture("Sword");
	if (sword_texture.has_value())
	{
		m_sword_texture = sword_texture.value();

		m_sword->setTexture(*m_sword_texture);

		m_sword->setTextureRect(sf::IntRect(0, 0, 15, 51));

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
		m_animation_time = 12.0f;
	}

	m_animation_time -= frameTime * 100.0f;
}

void LocalPlayer::DoDash()
{
	if (m_dash_time <= 0.0f)
	{
		m_dash_time = 10.0f;
	}
}


void LocalPlayer::DoKnockbackMove(float frameTime)
{
	sf::Vector2f delta = (m_position - m_goal_position);
	const float veclen = std::sqrt(delta.x * delta.x + delta.y * delta.y);
	if (veclen > 0.9f && veclen < 100.0f)
	{
		sf::Vector2f end_position = m_position + (sf::Vector2f(delta.x, delta.y) / (veclen * ((m_knockback_time / 30.0f))) * frameTime * 250.0f);

		World* world = entityManager->GetWorld();

		if (!world->DoesIntersectWall(GetBounds()))
		{
			m_position = end_position;

		}
	}
}

void LocalPlayer::TakeDamage(const int damage, const sf::Vector2f& pos, DamageType type)
{
	if ((globals->tick_count - m_last_damage_tick) > 256 && IsAlive())
	{
		m_health -= damage;
		m_knockback_time = 30.0f;
		m_damage_time = 60.0f;

		if (m_health <= 0)
		{
			m_death_time = 100.0f;
		}

		if (type == MeleeDamage)
		{

			World* world = entityManager->GetWorld();

			float angle = atan2f(pos.x - m_position.x, pos.y - m_position.y);

			sf::Vector2f delta = (pos - m_position);

			sf::Vector2f end = m_position - (sf::Vector2f(std::cos(angle), std::sin(angle))) * 1.0f;
			m_goal_position = end;
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
	if (m_attack_stage > 0.1f && m_sword->getGlobalBounds().intersects(target))
	{
		return true;
	}

	return false;
}

void LocalPlayer::DoAttack()
{
	if (m_attack_stage <= 0.0f && (globals->curtime - m_last_attack_time) > 0.5f)
	{
		m_attack_stage = 5.0f;
		m_last_attack_time = globals->curtime;
	}
}

void LocalPlayer::Process(float frameTime)
{

	if (!IsAlive() && m_death_time > 0.0f)
	{
		sf::Color col = m_respawn_background->getFillColor();
		float death_progress = m_death_time / 100.0f;
		col.a = std::min(255 - sf::Uint8(255 * death_progress), 255);
		m_respawn_background->setFillColor(col);
		m_death_time -= frameTime * 50.0f;
		return;
	}

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

	if (m_facing == Right && m_attack_stage <= 0.0f)
	{
		m_sword->move(sf::Vector2f(float(m_texture_sprite_size.width / 4 - 5), 0.0f));
		m_attack_angle = 180.0f;
	}
	if (m_facing == Left && m_attack_stage <= 0.0f)
	{
		m_sword->move(sf::Vector2f(float(-m_texture_sprite_size.width / 4 + 5), 0.0f));
		m_attack_angle = 0;
	}

	if (m_facing == Down && m_attack_stage <= 0.0f)
	{
		m_sword->move(sf::Vector2f(0, float(m_texture_sprite_size.height / 4 - 5)));
		m_attack_angle = -90;
	}
	if (m_facing == Up && m_attack_stage <= 0.0f)
	{
		m_sword->move(sf::Vector2f(0, float(-m_texture_sprite_size.height / 4 + 5)));
		m_attack_angle = 90;
	}

	if (m_knockback_time > 0.0f)
	{
		DoKnockbackMove(frameTime);
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

	if (m_damage_time > 0.0f)
	{
		m_damage_time -= frameTime * 100.0f;
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


	sf::Vector2f left = m_position + sf::Vector2f(20, 0);
	int left_intersection = world->GetIntersection(left);
	if (left_intersection & IntersectionDirection::InteresecRight)
	{
		if (m_velocity_goal.x > 0.0f)
		{
			m_velocity_goal.x = 0.0f;
		}
	}

	sf::Vector2f right = m_position - sf::Vector2f(20, 0);
	int right_intersection = world->GetIntersection(right);
	if (right_intersection & IntersectionDirection::InteresecLeft)
	{
		if (m_velocity_goal.x < 0.0f)
		{
			m_velocity_goal.x = 0.0f;
		}
	}

	sf::Vector2f up = m_position + sf::Vector2f(0, 20);
	int up_intersection = world->GetIntersection(up);
	if (up_intersection & IntersectionDirection::InteresecUp)
	{
		if (m_velocity_goal.y > 0.0f)
		{
			m_velocity_goal.y = 0.0f;
		}
	}

	sf::Vector2f down = m_position - sf::Vector2f(0, 80);
	int down_intersection = world->GetIntersection(down);
	if (down_intersection & IntersectionDirection::InteresecDown)
	{
		if (m_velocity_goal.y < 0.0f)
		{
			m_velocity_goal.y = 0.0f;
		}
	}


	m_velocity = m_velocity_goal;


	sf::Vector2f old_position = m_position;
	sf::Vector2f old_velocity = m_velocity;

	m_position = m_position + (m_velocity * frameTime);

	if (!world->TryMovement(this))
	{
		m_position = old_position;

	}

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


void LocalPlayer::RenderHud(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw && IsAlive())
	{
		//Draw current player health
		for (int i = 0; i < GetHealth(); i += 50)
		{
			renderWindow.draw(*m_heart_shape);

			m_heart_shape->setPosition(sf::Vector2f(float(i / (i > 0 ? 2 : 1) + 5), 10));
		}

		if (m_knockback_time > 0.0f)
		{
			sf::Color col = m_damage_indicator->getOutlineColor();
			float damage_progress = m_damage_time / 60.0f;
			col.a = sf::Uint8(255 * damage_progress);
			m_damage_indicator->setOutlineColor(col);
			renderWindow.draw(*m_damage_indicator);
		}
	}
	else if (m_render_state == RenderState::Draw && !IsAlive())
	{
		renderWindow.draw(*m_respawn_background);
		renderWindow.draw(*m_death_text);
	}
}


void LocalPlayer::SetPosition(const sf::Vector2f& pos)
{
	m_position = pos;
}
