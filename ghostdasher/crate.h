#pragma once
#include "entity.h"
#include "global-includes.h"
#include "globals.h"
#include "resourcemanager.h"

class Crate : public Entity
{
public:
	Crate();	
	Crate(const sf::Vector2f&);
	void Init();
	void Render(sf::RenderWindow& renderWindow);
	void Process(float frameTime);
	void LoadTextures();
	void Break();

	bool IsBroken()
	{
		return m_broken;
	}

	sf::FloatRect GetBounds()
	{
		if (m_broken)
		{
			return sf::FloatRect(m_position.x, m_position.y, 1, 1);
		}

		return m_shape->getGlobalBounds();
	}



private:
	std::unique_ptr<sf::Sprite> m_shape;
	sf::Texture* m_chest_texture;
	sf::Texture* m_potion_texture;
	sf::Vector2f m_goal_position;
	bool m_broken;
	float m_break_speed;
	sf::IntRect m_texture_sprite_size;
	float m_break_angle;
};