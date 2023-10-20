#pragma once
#include "hostile.h"
#include "projectile.h"

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

private:
	std::unique_ptr<sf::Sprite> m_shape;
	std::unique_ptr<sf::Text> m_spotted_text;
	sf::Texture* m_texture;
	float m_fire_time;
};