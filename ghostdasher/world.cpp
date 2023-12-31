#include "World.h"
#include "entitymanager.h"
#include "crate.h"
#include "door.h"

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
	m_waypoint_size = 100.0f;
	m_potential_objects.clear();
	m_points.clear();
}


void World::Reset()
{
	m_unwalkable_spaces.clear();
	m_potential_objects.clear();
	m_points.clear();
	m_is_boss_fight = false;
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
	float angle = atan2f(end.x - start.x, end.y - start.y) * 180 / M_PI;
	traceStart.setPosition(start);
	traceStart.setSize(sf::Vector2f(1, dist_len));
	traceStart.setRotation(-angle);

	traceStartBottom.setPosition(traceStart.getPosition());
	traceStartBottom.setSize(traceStart.getSize());
	traceStartBottom.setRotation(traceStart.getRotation());

	traceStart.setOrigin(sf::Vector2f(-40, -40));
	traceStartBottom.setOrigin(sf::Vector2f(40, 40));

	if (!IsInViewBounds(start) || !IsInViewBounds(end))
	{
		return false;
	}

	for (auto& it : GetUnwalkableSpaces())
	{
		if (traceStart.getGlobalBounds().intersects(it) && traceStartBottom.getGlobalBounds().intersects(it))
		{
			return false;
		}
	}

	return true;
}


bool World::IsInViewBounds(const sf::Vector2f& pos)
{
	const float half_w = m_view_size.x / 2;
	const float half_h = m_view_size.y / 2;

	if (pos.x < (m_view_position.x - half_w) || pos.x > (m_view_position.x + half_w))
	{
		return false;
	}

	if (pos.y < (m_view_position.y - half_h) || pos.y > (m_view_position.y + half_h))
	{
		return false;
	}


	return true;
}

bool World::DoesIntersectWall(const sf::FloatRect& target)
{
	const float shrinkScale = 0;
	for (auto& it : GetUnwalkableSpaces())
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



std::vector<Waypoint> World::GenerateWayPoints()
{
	if (m_points.size()) return m_points;
	int id = 0;
	for (float y = 0.0f; y < GetWorldBounds().y; y += m_waypoint_size)
	{
		for (float x = 0.0f; x < GetWorldBounds().x; x += m_waypoint_size)
		{
			if (DoesIntersectWall(sf::FloatRect(x, y, 30, 30)))
			{
				m_points.push_back(Waypoint(id, false,sf::Vector2f(x, y), (int)x, (int)y, 0, 0, 0));
				id++;
				continue;
			}

			m_points.push_back(Waypoint(id, true,sf::Vector2f(x, y), (int)x, (int)y, 0, 0, 0));
			id++;

		}
	}

	return m_points;
	
}

void World::UpdateUnwalkableSpaces()
{
	m_dynamic_unwalkable_spaces.clear();
	//Add extra positions which can be destroyed later. Walls, crates etc
	std::vector<sf::FloatRect> entity_pos;
	for (auto& it : entityManager->GetEntitiesByType<Crate*>())
	{
		if (!it->IsBroken())
		{
			entity_pos.push_back(sf::FloatRect(it->GetBounds().left, it->GetBounds().top, it->GetBounds().width, it->GetBounds().height));
		}
	}
	for (auto& it : entityManager->GetEntitiesByType<Door*>())
	{
		entity_pos.push_back(sf::FloatRect(it->GetPosition().x, it->GetPosition().y, it->GetBounds().width, it->GetBounds().height));
	}
	for (auto& it : entity_pos)
	{
		m_dynamic_unwalkable_spaces.push_back(it);
	}
}

std::vector<sf::FloatRect> World::GetUnwalkableSpaces()
{
	std::vector<sf::FloatRect> positions = m_unwalkable_spaces;
	if (m_dynamic_unwalkable_spaces.size())
	{
		for (auto& it : m_dynamic_unwalkable_spaces)
		{
			positions.push_back(it);
		}
	}

	return positions;
}

bool World::TryMovement(Entity* ent)
{
	sf::Vector2f position = ent->GetPosition();
	const float edgeSpeedScale = 5;
	sf::Vector2f new_position = position;
	sf::Vector2f new_velocity = ent->GetVelocity();
	sf::Vector2f new_velocity_goal = ent->GetVelocityGoal();

	const float velModifierBounce = 200;
	const float positionAdjust = 30.0f;

	const float scaleError = 30.0f;
	bool can_move = true;


	for (auto& it : GetUnwalkableSpaces())
	{

		float x = it.left - 30;
		float y = it.top - 30;
		float w = it.left + it.width + 30;
		float h = it.top + it.height + 30;

		if (position.x > x && position.x < w)
		{
			if (position.y > y && position.y < h)
			{
				can_move = false;
				new_velocity.x = -new_velocity.x;
				new_velocity.y = -new_velocity.y;

			}
		}	
	}

	ent->SetPosition(new_position);
	ent->SetVelocity(new_velocity);
	ent->SetGoalVelocity(new_velocity_goal);

	return can_move;
}
/*void World::UpdateIntersectingEntities()
{
	m_intersecting_entity_positions.clear();
	for (auto& it : entityManager->GetEntitiesByType<Hostile*>())
	{
		m_intersecting_entity_positions.push_back(sf::FloatRect(it->GetPosition().x, it->GetPosition().y, 5, 5));
	}

}*/

int World::GetIntersection(const sf::Vector2f& position, const sf::Vector2f& ent, bool first)
{
	const float velModifierBounce = 5;

	const float scaleError = 50.0f;
	int intersection = 0;

	for (auto& it : GetUnwalkableSpaces())
	{

		//if (it.left == ent.x && it.top && ent.x)
		//	continue;

		float x = it.left - 20;
		float y = it.top - 20;
		float w = it.left + it.width + 20;
		float h = it.top + it.height + 20;


		if (position.x > x && position.x < w && (position.y < h && position.y > y))
		{

			if (position.x > x && position.x < w - scaleError)
			{
				intersection |= IntersectionDirection::InteresecRight;
				if (first) return intersection;
			}

			else if (position.x > x + scaleError)
			{
				intersection |= IntersectionDirection::InteresecLeft;
				if (first) return intersection;

			}


			if (position.y > y && position.y < h - scaleError)
			{
				intersection |= IntersectionDirection::InteresecDown;
				if (first) return intersection;

			}

			else if (position.y > y + scaleError)
			{
				intersection |= IntersectionDirection::InteresecUp;
				if (first) return intersection;

			}
		}
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
void World::RenderExtra(sf::RenderWindow& renderWindow)
{
	if (m_render_state == RenderState::Draw)
	{
		for (const auto& it : m_potential_objects)
		{
			renderWindow.draw(*it->m_shape.get());
		}
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
	if (m_spawnpoints.size() == 0)
		return sf::Vector2f();

	int index = std::rand() % m_spawnpoints.size();

	if (index >= m_spawnpoints.size())
		return sf::Vector2f();

	return m_spawnpoints.at(index);
}

sf::Vector2f World::GetFurthestSpawnpoint(const sf::Vector2f& pos)
{
	float furtest_dist = 0.0f;
	sf::Vector2f furthest_point;
	for (auto it : m_spawnpoints)
	{
		sf::Vector2f delta = (it - pos);
		float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		if (dist > furtest_dist)
		{
			furtest_dist = dist;
			furthest_point = it;
		}
	}

	return furthest_point;
}

void World::LoadPotentialObject(const tmx::Object& object)
{
	bool have_tag = false;
	
	for (const auto& it : m_potential_objects)
	{
		if (!it->m_name.size())
			continue;

		if (it->m_name == object.getClass() && it->m_generated)
		{
			have_tag = true;
		}
	}

	if (!have_tag)
	{
		bool generate = rand() % 2;

		if (!object.getClass().size())
		{
			generate = rand() % 10 < 7;
		}

		if (generate)
		{
			if (object.getClass() == "crate")
			{
				entityManager->AddEntity(std::make_unique<Crate>(sf::Vector2f(object.getPosition().x, object.getPosition().y)));
			}
			else
			{
				std::unique_ptr<GeneratedObject> gen_object = std::make_unique<GeneratedObject>();

				gen_object->m_name = object.getClass();
				gen_object->m_bounds = sf::FloatRect(object.getAABB().left, object.getAABB().top, object.getAABB().width, object.getAABB().height);
				gen_object->m_generated = true;

				gen_object->m_shape = std::make_unique<sf::RectangleShape>();
				gen_object->m_shape->setSize(sf::Vector2f(gen_object->m_bounds.width, gen_object->m_bounds.height));
				gen_object->m_shape->setPosition(sf::Vector2f(gen_object->m_bounds.left, gen_object->m_bounds.top));
				gen_object->m_shape->setFillColor(sf::Color::Black);

				AddUnwalkableSpace(gen_object->m_bounds);

				m_potential_objects.push_back(std::move(gen_object));
			}
		}
	}
}