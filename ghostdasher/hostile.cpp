#include "Hostile.h"


Hostile::Hostile()
{
	m_shape = std::make_unique<sf::Sprite>();
	//m_shape->setSize(sf::Vector2f(40, 40));

	m_texture_sprite_size = sf::IntRect(0, 0, 24, 24);

	m_shape->setOrigin(sf::Vector2f(float(m_texture_sprite_size.width / 2), float(m_texture_sprite_size.height / 2)));

	m_position = sf::Vector2f((float)(rand() % 500), (float)(rand() % 500));
	m_goal_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
//	m_shape->setFillColor(sf::Color::Black);
//	m_shape->setOutlineColor(sf::Color::Red);
//	m_shape->setOutlineThickness(2);
	m_render_state = RenderState::Draw;
	m_movement_speed = 700.0f;
	m_type = EntityType::HostileEntity;
	m_health = 200;
	LoadTextures();
	m_shape->setScale(3.0f, 3.0f);

	m_total_animation_frames = 4;
	//m_path_finding = true;
}
void Hostile::LoadTextures()
{

	std::optional<sf::Texture*> run_texture = resourceManager->GetTexture("HostileIdle");
	if (run_texture.has_value())
	{
		m_texture = run_texture.value();
		m_shape->setTexture(*m_texture);
	}

}

void Hostile::PlayAnimation(float frameTime)
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


void Hostile::UpdatePath()
{
	if (m_path_finding)
	{
		m_path = pathFinder->GenerateBestPath(m_position, m_target_position);
		m_current_path_index = 0;
	}
}



void Hostile::DoRandomMovement(float frameTime)
{
	sf::Vector2f dist = (m_position - m_goal_position);
	float dist_len = std::sqrt(dist.x * dist.x + dist.y * dist.y);
	World* world = entityManager->GetWorld();

	const float moveDistance = 100.0f;

	if (dist_len < 10.0f)
	{
		int dir = std::rand() % 80;

		if (dir == 0)
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
		else if (dir == 1)
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
		else if (dir == 2)
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
		else if (dir == 3)
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
		if (m_position.x < m_goal_position.x)
		{
			m_position.x += 1.0f;
		}
		else if (m_position.x > m_goal_position.x)
		{
			m_position.x -= 1.0f;
		}
		else if (m_position.y < m_goal_position.y)
		{
			m_position.y += 1.0f;
		}
		else if (m_position.y > m_goal_position.y)
		{
			m_position.y -= 1.0f;
		}
		else
		{
			m_position = m_goal_position;
		}
	}
}

bool Hostile::CanSeePlayer()
{
	LocalPlayer* player = entityManager->GetLocalPlayer();
	World* world = entityManager->GetWorld();

	if (player && world && player->IsAlive())
	{
		if (world->IsPointVisible(player->GetPosition(), m_position))
		{
			m_target_position = player->GetPosition();
			m_last_spotted_player = globals->curtime;
			return true;
		}
	}

	return false;
}


void Hostile::GoToPosition(const sf::Vector2f& pos)
{
	if (m_move_delay < 0.1f)
	{
		if (m_path.size() && m_path_finding && m_knockback_time < 0.0f)
		{
			if (m_current_path_index < m_path.size())
			{
				sf::Vector2f cur_pos = m_position;
				m_position = m_path.at(m_current_path_index);
				if (cur_pos.x < m_position.x)
				{
					m_facing = Left;
				}
				else if (cur_pos.x > m_position.x)
				{
					m_facing = Right;
				}

				else if (cur_pos.y < m_position.y)
				{
					m_facing = Down;
				}
				else if (cur_pos.y > m_position.y)
				{
					m_facing = Up;
				}


				sf::Vector2f dist = (cur_pos - m_position);
				float dist_len = std::sqrt(dist.x * dist.x + dist.y * dist.y);
				if (dist_len > 10.0f)
				{
					m_position = cur_pos;
				}

				m_goal_position = m_position;

				m_current_path_index += 1;
			}
		}

		m_move_delay = 0.3f;
	}
}

void Hostile::TakeDamage(const int amount, FacingDirection dir)
{

	m_health -= amount;
	if (m_health <= 0)
	{
		Delete();
	}

	m_knockback_time = 5.0f;

	m_last_damage_tick = globals->tick_count;

	World* world = entityManager->GetWorld();

	const float moveDistance = 100.0f;

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

void Hostile::DoKnockbackMove()
{
	const float knockbackScale = 20.0f;
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

void Hostile::Process(float frameTime)
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
			m_path_finding = true;
		}
		else if ((globals->curtime - m_last_spotted_player) > 5.0f)
		{
			m_path_finding = false;
		}

		if(!m_path_finding)
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



	m_knockback_time -= frameTime * 100.0f;
	m_move_delay -= frameTime * 100.0f;
	m_direction_time -= frameTime * 100.0f;

	PlayAnimation(frameTime);

	m_last_position = m_position;

}

void Hostile::Render(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw)
	{
		renderWindow.draw(*m_shape.get());

	}
}

void Hostile::SetPosition(const sf::Vector2f& pos)
{
	m_position = pos;
}
