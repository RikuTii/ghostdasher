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

	sf::Vector2f GetVelocityGoal()
	{
		return m_velocity_goal;
	}

	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
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

	size_t GetEntityIndex()
	{
		return m_entity_index;
	}

	void SetEntityIndex(size_t index)
	{
		m_entity_index = index;
	}



	bool GetMarkedForDeletion()
	{
		return m_mark_deletion;
	}

	void Delete()
	{
		m_mark_deletion = true;
	}

protected:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_velocity_goal;
	RenderState m_render_state;
	EntityType m_type;
	size_t m_entity_index;
	bool m_mark_deletion;
private:
	std::unique_ptr<sf::RectangleShape> m_shape;
};