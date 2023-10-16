#include "pathfinder.h"

std::unique_ptr<PathFinder> pathFinder;
std::vector<sf::Vector2f> PathFinder::GenerateBestPath(const sf::Vector2f& entity, const sf::Vector2f& start, const sf::Vector2f& end)
{
	std::vector<sf::Vector2f> path_pos;
	std::vector<sf::Vector2f> path_pos1;

	std::vector<sf::Vector2f> path_neg;
	std::vector<sf::Vector2f> path_neg1;


	path_pos.clear();

	if (!m_world)
		return path_pos;

	m_tracking_entity = entity;
	SetGenMode(Negative);
	path_neg = GenerateHorizontalPath(start, end);
	path_neg1 = GenerateVerticalPath(start, end);
	SetGenMode(Positive);
	path_pos = GenerateHorizontalPath(start, end);
	path_pos1 = GenerateVerticalPath(start, end);
	SetGenMode(Normal);


	if (path_neg.size() && path_neg1.size() && path_pos.size() && path_pos1.size())
	{
		sf::Vector2f neg_dist = (path_neg.back() - end);
		float neg_dist_len = std::sqrt(neg_dist.x * neg_dist.x + neg_dist.y * neg_dist.y);
		sf::Vector2f neg_dist1 = (path_neg1.back() - end);
		float neg_dist_len1 = std::sqrt(neg_dist1.x * neg_dist1.x + neg_dist1.y * neg_dist1.y);

		sf::Vector2f pos_dist = (path_pos.back() - end);
		float pos_dist_len = std::sqrt(pos_dist.x * pos_dist.x + pos_dist.y * pos_dist.y);
		sf::Vector2f pos_dist1 = (path_pos1.back() - end);
		float pos_dist_len1 = std::sqrt(pos_dist1.x * pos_dist1.x + pos_dist1.y * pos_dist1.y);

		std::array<float, 4> dists = { neg_dist_len, neg_dist_len1, pos_dist_len,pos_dist_len1 };

		float lowest = 9999.0f;
		for (auto& it : dists)
		{
			if (it < lowest)
			{
				lowest = it;
			}
		}

		if (lowest == neg_dist_len)
			return path_neg;
		if (lowest == neg_dist_len1)
			return path_neg1;
		if (lowest == pos_dist_len)
			return path_pos;
		if (lowest == pos_dist_len1)
			return path_pos1;
	}


	return path_pos;
}
std::vector<sf::Vector2f> PathFinder::GenerateBestPathAStar(const sf::Vector2f& start_pos, const sf::Vector2f& end_pos)
{
	std::vector<sf::Vector2f> path_pos;

	static std::vector<Waypoint> waypoints = m_world->GenerateWayPoints();
	int waypoint_size = m_world->GetWaypointsWidth();


	auto get_distance = [](Waypoint a, Waypoint b)
		{
			int distance_x = std::abs(a.x - b.x);
			int distance_y = std::abs(a.y - b.y);

			if (distance_x > distance_y)
			{
				return 14 * distance_y + 10 * (distance_x - distance_y);
			}

			return 14 * distance_x + 10 * (distance_y - distance_x);
		};

	auto get_waypoint = [&waypoint_size](const sf::Vector2f& pos)
		{
			size_t pos_x = (int)(pos.x / 100.0f);
			size_t pos_y = (int)((pos.y / 100.0f)) * waypoint_size;
			return pos_x + pos_y;
		};

	auto get_neightbours = [&waypoint_size](const sf::Vector2f& pos)
		{
			std::vector<size_t> m_neighbours;
			for (int x = -1; x <= 1; x++)
			{
				for (int y = -1; y <= 1; y++)
				{
					size_t diag_pos_x = (int)(pos.x / 100.0f) + x;
					size_t diag_pos_y = (int)((pos.y / 100.0f) + y) * waypoint_size;
					m_neighbours.push_back(diag_pos_x + diag_pos_y);
				}
			}

			return m_neighbours;
		};


	auto generate_path = [](Waypoint start, Waypoint end)
		{
			std::list<Waypoint> m_path;
			Waypoint current = end;

			while (current.id != start.id)
			{
				m_path.push_back(current);
				if (current.parent_id == 0)
				{
					break;

				}
				if (waypoints.at(current.parent_id).parent_id == current.id)
				{
					break;
				}
				current = waypoints.at(current.parent_id);
			}

			m_path.reverse();

			return m_path;

		};

	std::vector<Waypoint> open_set;
	std::vector<Waypoint> closed_set;

	if (get_waypoint(start_pos) >= waypoints.size() || get_waypoint(end_pos) >= waypoints.size())
	{
		return path_pos;
	}

	Waypoint start = waypoints.at(get_waypoint(start_pos));
	start.parent_id = start.id;
	open_set.push_back(start);
	Waypoint end = waypoints.at(get_waypoint(end_pos));


	bool can_find_walkable_start = false;
	bool can_find_walkable_end = false;

	if (start.m_walkable)
	{
		can_find_walkable_start = true;
	}
	else
	{
		std::vector<size_t> neighbours = get_neightbours(start_pos);
		for (auto& it : neighbours)
		{
			if (waypoints.at(it).m_walkable)
			{
				can_find_walkable_start = true;
				start = waypoints.at(it);
			}
		}
	}

	if (end.m_walkable)
	{
		can_find_walkable_end = true;
	}
	else
	{
		std::vector<size_t> neighbours = get_neightbours(end_pos);
		for (auto& it : neighbours)
		{
			if (waypoints.at(it).m_walkable)
			{
				can_find_walkable_end = true;
				end = waypoints.at(it);
			}
		}
	}

	if (can_find_walkable_start && can_find_walkable_end)
	{
		while (open_set.size())
		{
			Waypoint node = open_set[0];
			for (int i = 1; i < open_set.size(); i++)
			{
				if (open_set[i].f_cost() < node.f_cost() || open_set[i].f_cost() == node.f_cost())
				{
					if (open_set[i].h_cost < node.h_cost)
					{
						node = open_set[i];
					}
				}
			}

			std::erase_if(open_set,
				[=](auto const& ptr) { return ptr.id == node.id; });

			closed_set.push_back(node);
			if (node.id == end.id)
			{
				auto m_path = generate_path(start, end);
				for (auto& it : m_path)
				{
					path_pos.push_back(it.m_position);
				}
				break;
			}

			std::vector<size_t> neighbours = get_neightbours(node.m_position);
			for (auto& it : neighbours)
			{
				if (it >= waypoints.size())
					continue;
				Waypoint neighbour = waypoints.at(it);
				auto closed_it = find_if(closed_set.begin(), closed_set.end(), [&](auto const& obj) { return obj.id == neighbour.id; });
				auto open_it = find_if(open_set.begin(), open_set.end(), [&](auto const& obj) { return obj.id == neighbour.id; });

				bool is_closed = closed_it != closed_set.end();

				if (!neighbour.m_walkable || is_closed)
				{
					continue;
				}

				bool is_open = open_it != open_set.end();

				int new_cost = node.g_cost + get_distance(node, neighbour);
				if (new_cost < neighbour.g_cost || !is_open)
				{
					neighbour.g_cost = new_cost;
					neighbour.h_cost = get_distance(neighbour, end);
					neighbour.parent_id = node.id;

					//this is needed so node parents are kept in check
					waypoints.at(neighbour.id).parent_id = node.id;

					if (!is_open)
					{
						open_set.push_back(neighbour);
					}

				}
			}


		}
	}

	return path_pos;
}



int PathFinder::AdjustMovement(const sf::Vector2f& start, const sf::Vector2f& end, sf::Vector2f& startPos)
{
	sf::Vector2f down_pos = sf::Vector2f(startPos.x - 20, startPos.y + 20);
	sf::Vector2f right_pos = sf::Vector2f(startPos.x + 20, startPos.y - 20);
	if (end.y < start.y)
	{
		down_pos.y = startPos.y - 40;
	}

	if (end.x < start.x)
	{
		right_pos.x = startPos.x - 20;
		down_pos.x = startPos.x + 20;
	}

	int dir = m_world->GetIntersection(startPos, m_tracking_entity);
	int dir_down = m_world->GetIntersection(down_pos, m_tracking_entity);
	int dir_right = m_world->GetIntersection(right_pos, m_tracking_entity);


	if ((dir & IntersectionDirection::InteresecRight) || (dir & IntersectionDirection::InteresecLeft))
	{
		for (int r = 0; r < m_adjust_count; r++)
		{
			if ((dir_down & IntersectionDirection::InteresecDown) && (dir_down & IntersectionDirection::InteresecUp))
				break;

			if (m_gen_mode == GenerateMode::Normal)
			{
				if (end.y > start.y)
					startPos.y += m_adjust_step;
				else
					startPos.y -= m_adjust_step;
			}
			else
			{
				startPos.y = startPos.y - (m_gen_mode == GenerateMode::Negative ? -m_adjust_step : m_adjust_step);
			}

			dir_right = m_world->GetIntersection(startPos, m_tracking_entity);
			if (!(dir_right & IntersectionDirection::InteresecRight) && !(dir_right & IntersectionDirection::InteresecLeft))
			{
				if (m_gen_mode == GenerateMode::Normal)
				{
					if (end.x > start.x)
						startPos.x += m_adjust_step;
					else
						startPos.x -= m_adjust_step;
				}
				else
				{
					startPos.x = startPos.x - (m_gen_mode == GenerateMode::Negative ? -m_adjust_step : m_adjust_step);
				}
				break;
			}

		}
	}

	if ((dir_down & IntersectionDirection::InteresecDown) || (dir_down & IntersectionDirection::InteresecUp))
	{

		for (int r = 0; r < m_adjust_count; r++)
		{
			if (m_gen_mode == GenerateMode::Normal)
			{
				if (end.x > start.x)
					startPos.x += m_adjust_step;
				else
					startPos.x -= m_adjust_step;
			}
			else
			{
				startPos.x = startPos.x - (m_gen_mode == GenerateMode::Negative ? -m_adjust_step : m_adjust_step);
			}
			dir_down = m_world->GetIntersection(startPos, m_tracking_entity);
			if (!(dir_down & IntersectionDirection::InteresecDown) && !(dir_down & IntersectionDirection::InteresecUp))
			{
				if (m_gen_mode == GenerateMode::Normal)
				{
					if (end.y > start.y)
						startPos.y += m_adjust_step;
					else
						startPos.y -= m_adjust_step;
				}
				else
				{
					startPos.y = startPos.y - (m_gen_mode == GenerateMode::Negative ? -m_adjust_step : m_adjust_step);
				}
				break;
			}
		}
	}

	return dir;
}

std::vector<sf::Vector2f> PathFinder::GenerateHorizontalPath(const sf::Vector2f& start, const sf::Vector2f& end)
{
	std::vector<sf::Vector2f> path;
	sf::Vector2f startPos = start;
	for (int d = 0; d < m_iteration_count; d++)
	{
		float x_delta = (startPos.x - end.x);
		float y_delta = (startPos.y - end.y);

		if (std::abs(x_delta) < 20.0f && std::abs(y_delta) < 20.0f)
			break;

		int dir = AdjustMovement(start, end, startPos);

		if (!(dir & IntersectionDirection::InteresecDown) && !(dir & IntersectionDirection::InteresecUp) && !(dir & IntersectionDirection::InteresecRight) && !(dir & IntersectionDirection::InteresecLeft))
		{
			if (x_delta > 1.0f)
			{
				startPos.x -= m_move_step;
			}
			else if (x_delta < -1.0f)
			{
				startPos.x += m_move_step;
			}
			else if (startPos.y < end.y)
			{
				startPos.y += m_move_step;
			}
			else if (startPos.y > end.y)
			{

				startPos.y -= m_move_step;
			}
		}

		if (m_world->DoesIntersectWall(sf::FloatRect(startPos.x, startPos.y, 1, 1)))
			break;


		path.push_back(startPos);

	}

	return path;
}

std::vector<sf::Vector2f> PathFinder::GenerateVerticalPath(const sf::Vector2f& start, const sf::Vector2f& end)
{
	std::vector<sf::Vector2f> path;
	sf::Vector2f startPos = start;
	for (int d = 0; d < m_iteration_count; d++)
	{
		float x_delta = (startPos.x - end.x);
		float y_delta = (startPos.y - end.y);

		if (std::abs(x_delta) < 20.0f && std::abs(y_delta) < 20.0f)
			break;

		int dir = AdjustMovement(start, end, startPos);

		if (!(dir & IntersectionDirection::InteresecDown) && !(dir & IntersectionDirection::InteresecUp) && !(dir & IntersectionDirection::InteresecRight) && !(dir & IntersectionDirection::InteresecLeft))
		{
			if (y_delta > 1.0f)
			{
				startPos.y -= m_move_step;
			}
			else if (y_delta < -1.0f)
			{
				startPos.y += m_move_step;
			}
			else if (startPos.x < end.x)
			{
				startPos.x += m_move_step;
			}
			else if (startPos.x > end.x)
			{

				startPos.x -= m_move_step;
			}
		}


		if (m_world->DoesIntersectWall(sf::FloatRect(startPos.x, startPos.y, 1, 1)))
			break;

		path.push_back(startPos);

	}

	return path;
}
