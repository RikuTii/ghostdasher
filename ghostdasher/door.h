#pragma once
#include "entity.h"
class Door : public Entity
{
public:
	Door();
	Door(const sf::Vector2f&);

	void Render(sf::RenderWindow& renderWindow);
	void Process(float frameTime);
	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}

	void SetOpen();

private:
	void Init();
	std::unique_ptr<sf::RectangleShape> m_shape;
	bool m_is_open;
	float m_open_timer;
};