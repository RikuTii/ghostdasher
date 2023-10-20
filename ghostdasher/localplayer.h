#pragma once
#include "global-includes.h"
#include "entity.h"
#include "resourcemanager.h"

class LocalPlayer : public Entity
{
public:
	LocalPlayer();
	~LocalPlayer() {};
	void Render(sf::RenderWindow& renderWindow);
	void RenderHud(sf::RenderWindow& renderWindow);
	void SetPosition(const sf::Vector2f& pos);
	void Process(float frameTime);
	void ProcessMovement(float frameTime);
	void DoKnockbackMove(float frameTime);
	void LoadTextures();
	bool CheckSwordCollision(const sf::FloatRect& target);
	void DoDash();
	void TakeDamage(const int damage, const sf::Vector2f& pos, DamageType type = MeleeDamage);
	void PlayAnimation(float frameTime);
	void DoAttack();

	void Respawn()
	{
		m_health = 200;
	}

	bool Heal(int amount)
	{
		bool can_pick_up = m_health < m_max_health;
		m_health += amount;
		m_health = std::min(m_health, m_max_health);

		return can_pick_up;
	}
	
	bool IsAlive()
	{
		return m_health > 0;
	}

	int GetHealth()
	{
		return m_health;
	}

	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}

	void SetFacing(FacingDirection dir)
	{
		m_facing = dir;
	}

	FacingDirection GetFacing()
	{
		return m_facing;
	}

	float GetMovementSpeed()
	{
		return m_movement_speed;
	}

	enum Animation
	{
		Idle,
		Run
	};

private:
	std::unique_ptr<sf::Sprite> m_shape;
	std::unique_ptr<sf::CircleShape> m_heart_shape;
	std::unique_ptr<sf::CircleShape> m_damage_indicator;

	sf::Texture* m_texture;
	sf::Texture* m_idle_texture;
	std::unique_ptr<sf::RectangleShape> m_sword;
	sf::IntRect m_texture_sprite_size;
	sf::Vector2f m_sprite_scale;
	Animation m_last_animation;
	Animation m_current_animation;
	float m_movement_speed;
	float m_attack_stage;
	float m_attack_angle;
	float m_dash_time;
	float m_animation_time;
	int m_animation_frame;
	int m_total_animation_frames;
	sf::Vector2f m_last_velocity;
	sf::Vector2f m_goal_position;
	int m_health;
	int m_max_health;
	float m_last_attack_time;
	float m_damage_time;
	int m_last_damage_tick;
	int m_last_attack_tick;
	float m_knockback_time;
	FacingDirection m_facing;
};