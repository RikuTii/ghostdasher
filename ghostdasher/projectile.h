#pragma once
#include "entity.h"
#include "world.h"
#include "entitymanager.h"
#include "localplayer.h"
#include "hostile.h"

class Projectile : public Entity
{
public:
	Projectile();
	Projectile(const sf::Vector2f&, const sf::Vector2f&);
	~Projectile()
	{

	}


	void Explode();
	void Render(sf::RenderWindow& renderWindow);
	void LoadTextures();
	void PlayAnimation(float frameTime);
	void Process(float frameTime);
	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}



private:
	void Init();
	std::unique_ptr<sf::Sprite> m_shape;
	sf::Texture* m_projectile_texture;
	sf::Texture* m_exploded_texture;
	bool m_should_collide;
	bool m_should_update;
	float m_lifetime;
	float m_life;
	bool m_should_hoam_target;
	bool m_should_explode;
	bool m_exploded;
	float m_animation_time;
	int m_animation_frame;
	int m_total_animation_frames;
	int m_min_animation_frame;
	sf::IntRect m_texture_sprite_size;
	sf::Vector2f m_target_position;
	size_t m_target_entity_index;
};