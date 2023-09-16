#pragma once
#include "global-includes.h"
#include "types.h"
class Entity
{
public:
	Entity();
	void Render(sf::RenderWindow& renderWindow);
	void SetPosition(const sf::Vector2f& pos);
	void Process(float frameTime);
	void SetVelocity(const sf::Vector2f& vel);
	sf::Vector2f GetPosition()
	{
		return m_position;
	}

	void ResetOrigin()
	{
		m_position = sf::Vector2f(0, 0);
	}

	bool ShouldRender()
	{
		return m_render_state == RenderState::Draw;
	}

	void SetRenderState(RenderState state)
	{
		m_render_state = state;
	}
protected:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_velocityGoal;
	RenderState m_render_state;
private:
	std::unique_ptr<sf::RectangleShape> m_shape;
};