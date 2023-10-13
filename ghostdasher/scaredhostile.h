#pragma once
#include "hostile.h"


class ScaredHostile : public Hostile
{
public:
	ScaredHostile();
	void LoadTextures();
	void Process(float frameTime);
	void Render(sf::RenderWindow& renderWindow);
	void PlayAnimation(float frameTime);
	void TakeDamage(const int amount, FacingDirection dir);

	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}


private:
	std::unique_ptr<sf::Sprite> m_shape;
	std::unique_ptr<sf::Text> m_spotted_text;
	sf::Texture* m_texture;
	float m_retreate_time;
};