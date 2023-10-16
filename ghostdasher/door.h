#pragma once
#include "entity.h"
class Door : public Entity
{
public:
	Door();
	Door(const sf::Vector2f&,const sf::Vector2f&);

	void Render(sf::RenderWindow& renderWindow);
	void Process(float frameTime);
	sf::FloatRect GetBounds()
	{
		return m_shape->getGlobalBounds();
	}

	void SetOpen();

	bool IsOpen()
	{
		return m_is_open;
	}

	void SetTrigger(const sf::FloatRect& pos)
	{
		m_trigger_position = pos;
	}


private:
	void Init();
	std::unique_ptr<sf::RectangleShape> m_shape;
	bool m_is_open;
	sf::FloatRect m_trigger_position;
	sf::Vector2f m_origin_position;
	bool m_active_trigger;
	bool m_opening;
	bool m_closing;
	float m_open_speed;
};