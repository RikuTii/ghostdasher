#pragma once
#pragma once
#include "global-includes.h"
#include "entity.h"

class LocalPlayer : public Entity
{
public:
	LocalPlayer();
	virtual void Render(sf::RenderWindow& renderWindow);
	virtual void SetPosition(const sf::Vector2f& pos);
	virtual void Process(float frameTime);
	virtual void SetVelocity(const sf::Vector2f& vel);

	bool CheckSwordCollision(const sf::FloatRect& target);
	void DoAttack()
	{
		m_attack_stage = 5.0f;
	}

	void SetFacing(FacingDirection dir)
	{
		m_facing = dir;
	}

	float GetMovementSpeed()
	{
		return m_movement_speed;
	}

private:
	std::unique_ptr<sf::RectangleShape> m_shape;
	std::unique_ptr<sf::RectangleShape> m_sword;
	float m_movement_speed;
	float m_attack_stage;
	float m_attack_angle;
	sf::Vector2f m_last_velocity;
	FacingDirection m_facing;
};