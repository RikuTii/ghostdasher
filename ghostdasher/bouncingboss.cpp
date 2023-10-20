#include "bouncingboss.h"
#include "window.h"

BouncingBoss::BouncingBoss()
{
	Init();
}

BouncingBoss::BouncingBoss(const sf::Vector2f& pos)
{
	Init();
	m_position = pos;
}

void BouncingBoss::Init()
{
	m_shape = std::make_unique<sf::Sprite>();
	m_texture_sprite_size = sf::IntRect(0, 0, 64, 64);
	m_shape->setOrigin(sf::Vector2f(float(m_texture_sprite_size.width / 2), float(m_texture_sprite_size.height / 2)));
	m_position = sf::Vector2f((float)(rand() % 500), (float)(rand() % 500));
	m_goal_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
	m_render_state = RenderState::Draw;
	m_movement_speed = 700.0f;
	m_type = EntityType::BouncingBossEntity;

	m_category = EntityCategory::CategoryBoss;

	m_total_health = 1000;

	m_movement_speed = 350.0f;

	m_health = static_cast<int>(m_total_health);

	LoadTextures();
	m_shape->setScale(5.0f, 5.0f);

	m_shape->setColor(sf::Color::Yellow);

	m_total_animation_frames = 6;

	m_health_background = std::make_unique<sf::RectangleShape>();

	m_health_background->setFillColor(sf::Color::Black);
	m_health_background->setSize(sf::Vector2f(600, 20));
	m_health_background->setPosition(sf::Vector2f(window->getDefaultView().getSize().x / 2 - m_health_background->getSize().x / 2, 20));
	m_health_background->setOutlineColor(sf::Color::White);
	m_health_background->setOutlineThickness(1.0f);

	m_health_fill = std::make_unique<sf::RectangleShape>();
	m_health_fill->setFillColor(sf::Color::Green);
	m_health_fill->setSize(sf::Vector2f(600, 20));
	m_health_fill->setPosition(sf::Vector2f(window->getDefaultView().getSize().x / 2 - m_health_background->getSize().x / 2, 20));


	m_start_time = 55.0f;

	m_rotation_time = 0.0f;

	do
	{
		m_bounce_angle = (std::rand() % 360) * 2 * M_PI / 360;
	} while (std::abs(std::cos(m_bounce_angle)) < 0.7f);
}
void BouncingBoss::LoadTextures()
{

	std::optional<sf::Texture*> run_texture = resourceManager->GetTexture("Boss");
	if (run_texture.has_value())
	{
		m_texture = run_texture.value();
		m_shape->setTexture(*m_texture);
		sf::IntRect texture_rect = m_texture_sprite_size;
		texture_rect.left = (texture_rect.width * m_animation_frame);
		m_shape->setTextureRect(texture_rect);
	}

}

void BouncingBoss::PlayAnimation(float frameTime)
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

void BouncingBoss::TakeDamage(const int amount, const sf::Vector2f& dir)
{
	m_health -= amount;
	if (m_health <= 0)
	{
		Delete();
	}

	m_last_damage_tick = globals->tick_count;

	sf::Vector2f delta = (m_position - dir);

	float angle = atan2f(dir.x - m_position.x, dir.y - m_position.y) * (180 / M_PI);

	float perc = (m_health / m_total_health);

	m_health_fill->setSize(sf::Vector2f(m_health_background->getSize().x - (m_health_background->getSize().x * (1.0f - perc)), m_health_fill->getSize().y));

	m_bounce_angle = -((angle + 90.0f) * (M_PI / 180.0f));

	m_bounce_knockback = 35.0f;
}



void BouncingBoss::Process(float frameTime)
{
	m_shape->setPosition(m_position);

	World* world = entityManager->GetWorld();

	float factor = m_movement_speed * frameTime;

	if (m_bounce_knockback > 0.0f)
	{
		factor *= m_bounce_knockback / 4.0f;
	}

	m_position = sf::Vector2f(m_position.x + (std::cos(m_bounce_angle) * factor), m_position.y + std::sin(m_bounce_angle) * factor);

	sf::Vector2f left_intersect = m_position + sf::Vector2f(50, 0);
	sf::Vector2f right_intersect = m_position - sf::Vector2f(50, 0);
	sf::Vector2f up_intersect = m_position + sf::Vector2f(0, 50);
	sf::Vector2f down_intersect = m_position - sf::Vector2f(0, 50);

	int intersection_left = world->GetIntersection(left_intersect);
	int intersection_right = world->GetIntersection(right_intersect);
	int intersection_up = world->GetIntersection(up_intersect);
	int intersection_down = world->GetIntersection(down_intersect);

	if (intersection_left)
	{
		m_bounce_angle = M_PI + m_bounce_angle + (std::rand() % 20) * M_PI / 180;
		m_position = m_position - sf::Vector2f(10, 0);
	}

	if (intersection_right)
	{
		m_bounce_angle = M_PI - m_bounce_angle + (std::rand() % 20) * M_PI / 180;
		m_position = m_position + sf::Vector2f(10, 0);
	}

	if (intersection_up || intersection_down)
	{
		m_bounce_angle = -m_bounce_angle;
		if (intersection_up)
			m_position = m_position - sf::Vector2f(0, 10);
		else
			m_position = m_position + sf::Vector2f(0, 10);
	}

	m_shape->setRotation(fmodf(m_rotation_time, 360.0f));

	m_rotation_time += frameTime * 100.0f;

	if (m_bounce_knockback > 0.0f)
	{
		m_bounce_knockback -= frameTime * 100.0f;
	}

	m_last_position = m_position;


	PlayAnimation(frameTime);
}


void BouncingBoss::Render(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw)
	{
		renderWindow.draw(*m_shape.get());
	}
}

void BouncingBoss::RenderHud(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw)
	{
		renderWindow.draw(*m_health_background.get());
		renderWindow.draw(*m_health_fill.get());
	}
}