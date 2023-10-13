#pragma once
#include "entity.h"
class Crate : public Entity
{
public:
	Crate();
	void Render(sf::RenderWindow& renderWindow);
	void Process(float frameTime);
	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}

private:
	std::unique_ptr<sf::RectangleShape> m_shape;
};