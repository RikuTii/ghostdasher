#pragma once
#include "hostile.h"


class BouncingBoss : public Hostile
{
public:
	BouncingBoss();
	BouncingBoss(const sf::Vector2f&);

	void Init();
	void LoadTextures();
	void Process(float frameTime);
	void Render(sf::RenderWindow& renderWindow);
	void RenderHud(sf::RenderWindow& renderWindow);

	void PlayAnimation(float frameTime);
	void TakeDamage(const int amount, const sf::Vector2f& dir);

	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}

	float GetBounceAngle()
	{
		return m_bounce_angle;
	}

	void SetBounceAngle(float angle)
	{
		m_bounce_angle = angle;
	}


private:
	std::unique_ptr<sf::Sprite> m_shape;
	std::unique_ptr<sf::RectangleShape> m_health_background;
	std::unique_ptr<sf::RectangleShape> m_health_fill;
	sf::Texture* m_texture;
	float m_rotation_time;
	float m_bounce_angle;
	float m_total_health;
	float m_bounce_knockback;
	float m_start_time;
};