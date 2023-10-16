#pragma once
#include "hostile.h"


class FiringHostile : public Hostile
{
public:
	FiringHostile();
	FiringHostile(const sf::Vector2f&);

	void Init();
	void LoadTextures();
	void Process(float frameTime);
	void Render(sf::RenderWindow& renderWindow);
	void PlayAnimation(float frameTime);
	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}

	struct Projectile
	{
		bool m_should_collide;
		bool m_should_update;
		float m_lifetime;
		float m_life;
		sf::Vector2f m_current_position;
		sf::Vector2f m_target_position;
		std::unique_ptr<sf::RectangleShape> m_projectile_texture;
	};
private:
	std::unique_ptr<sf::Sprite> m_shape;
	std::vector<std::unique_ptr<Projectile>> m_projectiles;
	std::unique_ptr<sf::Text> m_spotted_text;
	sf::Texture* m_texture;
	float m_fire_time;
};