#include "World.h"


World::World()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_bounds = sf::Vector2f(3000, 3000);
	m_shape->setSize(m_bounds);
	m_shape->setOrigin(sf::Vector2f(0, 0));
	m_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color(80,80,80));
	SetRenderState(RenderState::Draw);
	InitalizeWalkableSpace();
}


void World::InitalizeWalkableSpace()
{
	m_unwalkable_spaces.push_back(sf::FloatRect{ 400,400,300,300 });
	m_unwalkable_spaces.push_back(sf::FloatRect{ 200,200,150,300 });
	m_unwalkable_spaces.push_back(sf::FloatRect{ 200,600,300,80 });
}


void World::AddUnwalkableSpace(const sf::FloatRect& space)
{
	m_unwalkable_spaces.push_back(space);
}

bool World::DoesIntersectWall(const sf::FloatRect& target)
{
	const float shrinkScale = 50.0f;
	for (auto &it : m_unwalkable_spaces)
	{
		sf::FloatRect shrunk = it;
		shrunk.left += shrinkScale;
		shrunk.top += shrinkScale;
		shrunk.width -= shrinkScale;
		shrunk.height -= shrinkScale;

		if (shrunk.intersects(target))
		{
			return true;
		}
	}

	return false;
}

void World::TryMovement(Entity* ent)
{
	sf::Vector2f position = ent->GetPosition();
	const float edgeSpeedScale = 5;
	sf::Vector2f new_position = position;
	sf::Vector2f new_velocity = ent->GetVelocity();
	sf::Vector2f new_velocity_goal = ent->GetVelocityGoal();

	if (position.x < 0.0f)
	{
		new_position.x = 0.0f;
		new_velocity.x = edgeSpeedScale;
		new_velocity_goal.x = edgeSpeedScale;

	}
	if (position.x > m_bounds.x)
	{
		new_position.x = m_bounds.x;
		new_velocity.x = -edgeSpeedScale;
		new_velocity_goal.x = -edgeSpeedScale;

	}

	if (position.y < 0.0f)
	{
		new_position.y = 0.0f;
		new_velocity.y = edgeSpeedScale;
		new_velocity_goal.y = edgeSpeedScale;

	}
	if (position.y > m_bounds.y)
	{
		new_position.y = m_bounds.y;
		new_velocity.y = -edgeSpeedScale;
		new_velocity_goal.y = -edgeSpeedScale;

	}

	const float velModifierBounce = 5;

	const float scaleError = ent->GetBounds().height;

	for (auto it : m_unwalkable_spaces)
	{

		float x = it.left - 20;
		float y = it.top - 20;
		float w = it.left + it.width + 20;
		float h = it.top + it.height + 20;


		if (position.x > x && position.x < w && (position.y < h && position.y > y))
		{

			if (position.x > x && position.x < w - scaleError)
			{
				new_velocity.x = -velModifierBounce;
			}

			else if (position.x > x + scaleError)
			{
				new_velocity.x = velModifierBounce;
			}


			if (position.y > y && position.y < h - scaleError)
			{
				new_velocity.y = -velModifierBounce;
			}

			else if (position.y > y + scaleError)
			{
				new_velocity.y = velModifierBounce;
			}
		}
	}

	ent->SetPosition(new_position);
	ent->SetVelocity(new_velocity);
	ent->SetGoalVelocity(new_velocity_goal);

}


void World::Process(float frameTime)
{
	m_shape->setPosition(m_position);

	m_position = m_position;

}

void World::Render(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw)
	{
		renderWindow.draw(*m_shape.get());
	}
}

void World::SetPosition(const sf::Vector2f& pos)
{
	m_position = pos;
}

