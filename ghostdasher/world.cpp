#include "World.h"
#include "entitymanager.h"

World::World()
{
	m_shape = std::make_unique<sf::RectangleShape>();
	m_bounds = sf::Vector2f(3000, 3000);
	m_shape->setSize(m_bounds);
	m_shape->setOrigin(sf::Vector2f(0, 0));
	m_position = sf::Vector2f(0, 0);
	m_shape->setPosition(m_position);
	m_shape->setFillColor(sf::Color(80, 80, 80));
	SetRenderState(RenderState::Draw);
	m_unwalkable_spaces.clear();
	//InitalizeWalkableSpace();
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


bool World::IsPointVisible(const sf::Vector2f& start, const sf::Vector2f& end)
{
	sf::RectangleShape traceStart;
	sf::RectangleShape traceStartBottom;
	sf::Vector2f dist = (start - end);
	float dist_len = std::sqrt(dist.x * dist.x + dist.y * dist.y);
	float angle = atan2f(end.x - start.x, end.y - start.y) * 180 / 3.14f;
	traceStart.setPosition(start);
	traceStart.setSize(sf::Vector2f(1, dist_len));
	traceStart.setRotation(-angle);

	traceStartBottom.setPosition(traceStart.getPosition());
	traceStartBottom.setSize(traceStart.getSize());
	traceStartBottom.setRotation(traceStart.getRotation());

	traceStart.setOrigin(sf::Vector2f(-40, -40));
	traceStartBottom.setOrigin(sf::Vector2f(40, 40));

	for (auto& it : m_unwalkable_spaces)
	{
		if (traceStart.getGlobalBounds().intersects(it) && traceStartBottom.getGlobalBounds().intersects(it))
		{
			return false;
		}
	}

	return true;
}

bool World::DoesIntersectWall(const sf::FloatRect& target)
{
	const float shrinkScale = 0;
	for (auto& it : m_unwalkable_spaces)
	{
		sf::FloatRect shrunk = it;
		shrunk.left += shrinkScale;
		shrunk.top += shrinkScale;
		shrunk.width -= shrinkScale;
		shrunk.height -= shrinkScale;

		if (target.intersects(shrunk))
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
	const float positionAdjust = 30.0f;

	const float scaleError = 30.0f;

	for (auto& it : m_unwalkable_spaces)
	{

		float x = it.left - 20;
		float y = it.top - 20;
		float w = it.left + it.width + 20;
		float h = it.top + it.height + 20;


		if (position.x > x && position.x < w && (position.y < h && position.y > y))
		{

			if (position.x > x && position.x < w - 100)
			{
				float diff_x = (position.x - x);
				float diff_y = (position.y - y);

				if (position.y > y && position.y < h - 100)
				{


					if (position.y > y)
					{
						if (diff_x < diff_y)
							new_position.x = x;
						else
							new_position.y = y;

					}
					else
						new_position.x = x;
				}
				else
				{
					float diff_x = (position.x - w);
					float diff_y = (position.y - h);

					if (position.x > w)
					{
						if (diff_y < diff_x)
							new_position.y = h;
						else
							new_position.x = w;

					}
					else
						new_position.y = h;

				}
	

			}
			else if (position.y > y && position.y < h - 100)
			{
				float diff_x = (position.x - x);
				float diff_y = (position.y - y);

				if (position.x > x && position.x < w - 100)
				{
					if (position.x > x)
					{
						if (diff_y < diff_x)
							new_position.y = y;
						else
							new_position.x = x;

					}
					else
						new_position.y = y;
				}
				else
				{
					float diff_x = (position.x - w);
					float diff_y = (position.y - h);

					if (position.y > h)
					{
						if (diff_x < diff_y)
							new_position.x = w;
						else
							new_position.y = h;

					}
					else
						new_position.x = w;
				}
			}
			else if (position.x > x)
			{
				float diff_x = (position.x - w);
				float diff_y = (position.y - h);

				if (position.y > h)
				{
					if (diff_x < diff_y)
						new_position.x = w;
					else
						new_position.y = h;

				}
				else
					new_position.x = w;
			}
			else
			{
				float diff_x = (position.x - w);
				float diff_y = (position.y - h);

				if (position.x > w)
				{
					if (diff_y < diff_x)
						new_position.y = h;
					else
						new_position.x = w;

				}
				else
					new_position.y = h;
			
			}



			if (position.x > x && position.x < w - scaleError)
			{
				new_velocity.x = -velModifierBounce;
				new_velocity_goal.x = -velModifierBounce;
			}

			else if (position.x > x + scaleError)
			{
				new_velocity.x = velModifierBounce;
				new_velocity_goal.x = velModifierBounce;
			}

			if (position.y > y && position.y < h - scaleError)
			{
				new_velocity.y = -velModifierBounce;
				new_velocity_goal.y = -velModifierBounce;
			}

			else if (position.y > y + scaleError)
			{
				new_velocity.y = velModifierBounce;
				new_velocity_goal.y = velModifierBounce;

			}
		}
	}

	ent->SetPosition(new_position);
	ent->SetVelocity(new_velocity);
	ent->SetGoalVelocity(new_velocity_goal);

}


int World::GetIntersection(const sf::Vector2f& position, Entity* ent)
{
	const float velModifierBounce = 5;

	const float scaleError = 50.0f;
	int intersection = 0;

	std::vector<sf::FloatRect> entity_pos;
	for (auto& it : entityManager->GetEntitiesByType<Hostile*>())
	{
		if (it == ent)
			continue;

		entity_pos.push_back(sf::FloatRect(it->GetPosition().x, it->GetPosition().y, 5, 5));
	}


	std::vector<sf::FloatRect> positions = m_unwalkable_spaces;
	positions.insert(positions.end(), entity_pos.begin(), entity_pos.end());

	for (auto& it : positions)
	{

		float x = it.left - 20;
		float y = it.top - 20;
		float w = it.left + it.width + 20;
		float h = it.top + it.height + 20;


		if (position.x > x && position.x < w && (position.y < h && position.y > y))
		{

			if (position.x > x && position.x < w - scaleError)
			{
				intersection |= IntersectionDirection::InteresecRight;
			}

			else if (position.x > x + scaleError)
			{
				intersection |= IntersectionDirection::InteresecLeft;

			}


			if (position.y > y && position.y < h - scaleError)
			{
				intersection |= IntersectionDirection::InteresecDown;

			}

			else if (position.y > y + scaleError)
			{
				intersection |= IntersectionDirection::InteresecUp;

			}
		}
	}

	if (position.x < 0.0f)
	{
		intersection |= IntersectionDirection::InteresecLeft;
	}
	if (position.x > m_bounds.x)
	{
		intersection |= IntersectionDirection::InteresecRight;
	}
	if (position.y < 0.0f)
	{
		intersection |= IntersectionDirection::InteresecUp;
	}
	if (position.y > m_bounds.y)
	{
		intersection |= IntersectionDirection::InteresecDown;
	}

	return intersection;
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



void World::AddSpawnPoint(const sf::Vector2f& point)
{
	m_spawnpoints.push_back(point);
}

sf::Vector2f World::GetRandomSpawnPoint()
{
	int index = std::rand() % m_spawnpoints.size();

	if (index >= m_spawnpoints.size())
		return sf::Vector2f();

	return m_spawnpoints.at(index);
}