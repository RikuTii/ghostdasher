#pragma once
#pragma once
#include "global-includes.h"
#include "entity.h"

class World : public Entity
{
public:
	World();
	virtual void Render(sf::RenderWindow& renderWindow);
	virtual void SetPosition(const sf::Vector2f& pos);
	virtual void Process(float frameTime);
	sf::Vector2f GetBounds()
	{
		return m_bounds;
	}
private:
	std::unique_ptr<sf::RectangleShape> m_shape;
	sf::Vector2f m_position;
	sf::Vector2f m_bounds;
};