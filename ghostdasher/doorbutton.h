#pragma once
#include "entity.h"
class DoorButton : public Entity
{
public:
	DoorButton();
	DoorButton(const sf::Vector2f&);

	void Render(sf::RenderWindow& renderWindow);
	void Process(float frameTime);
	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}

	void SetPressed()
	{
		m_is_pressed = true;
	}

	bool IsPressed()
	{
		return m_is_pressed;
	}

private:
	void Init();
	std::unique_ptr<sf::RectangleShape> m_shape;
	bool m_is_pressed;
};