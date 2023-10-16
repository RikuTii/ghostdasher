#pragma once
#include "global-includes.h"
#include "entity.h"
#include "world.h"
#include "entitymanager.h"
#include "pathfinder.h"
#include "globals.h"

class Hostile : public Entity
{
public:
	Hostile();
	Hostile(const sf::Vector2f&);
	virtual ~Hostile() {};
	void Render(sf::RenderWindow& renderWindow);
	void SetPosition(const sf::Vector2f& pos);
	void Process(float frameTime);
	void DoRandomMovement(float frameTime);
	virtual void TakeDamage(const int amount, FacingDirection dir);
	void CalculateKnockback(FacingDirection dir);
	void GoToPosition();
	bool CanSeePlayer();
	virtual void Init();
	void UpdatePath();
	void DoKnockbackMove();
	virtual void PlayAnimation(float frameTime);
	virtual void LoadTextures();

	void SetFacing(FacingDirection dir)
	{
		m_facing = dir;
	}

	FacingDirection GetFacing()
	{
		return m_facing;
	}

	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}

	float GetMovementSpeed()
	{
		return m_movement_speed;
	}

	void UpdateTargetPosition(const sf::Vector2f& pos)
	{
		m_target_position = pos;
	}

	void SetGoalPosition(const sf::Vector2f& pos)
	{
		m_goal_position = pos;
	}

	bool IsPathFinding()
	{
		return m_path_finding;
	}


	void SetPath(std::vector<sf::Vector2f> path)
	{
		m_path = path;
	}

	void ResetPath()
	{
		m_current_path_index = 0;
	}

	sf::Vector2f GetTargetPosition()
	{
		return m_target_position;
	}

	enum Animation
	{
		Idle,
		Run
	};

	enum SpottedState
	{
		Spotted,
		Lost,
		None
	};

protected:
	sf::Vector2f m_goal_position;
	sf::Vector2f m_last_velocity;
	sf::Vector2f m_target_position;
	std::vector<sf::Vector2f> m_path;
	sf::Vector2f m_last_position;
	int m_current_path_index;
	bool m_path_finding;
	int m_health;
	FacingDirection m_facing;
	float m_move_delay;
	float m_direction_time;
	float m_last_spotted_player;
	float m_knockback_time;
	int m_last_damage_tick;
	sf::IntRect m_texture_sprite_size;
	Animation m_last_animation;
	Animation m_current_animation;
	float m_animation_time;
	int m_animation_frame;
	int m_total_animation_frames;
	int m_min_animation_frame;
	SpottedState m_spot_state;
	float m_spotted_time;
	float m_movement_speed;
private:
	std::unique_ptr<sf::Sprite> m_shape;
	std::unique_ptr<sf::Text> m_spotted_text;
	sf::Texture* m_texture;
};