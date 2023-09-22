#pragma once
#pragma once
#include "global-includes.h"
#include "entity.h"

class LocalPlayer : public Entity
{
public:
	LocalPlayer();
	~LocalPlayer() {};
	void Render(sf::RenderWindow& renderWindow);
	void SetPosition(const sf::Vector2f& pos);
	void Process(float frameTime);

	bool CheckSwordCollision(const sf::FloatRect& target);
	void DoDash();
	void DoAttack()
	{
		m_attack_stage = 5.0f;
	}

	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
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
	float m_dash_time;
	sf::Vector2f m_last_velocity;
	FacingDirection m_facing;
};