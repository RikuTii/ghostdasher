#include "pathfinder.h"

std::unique_ptr<PathFinder> pathFinder;
std::vector<sf::Vector2f> PathFinder::GenerateBestPath(Entity*entity, const sf::Vector2f& start, const sf::Vector2f& end)
{
	std::vector<sf::Vector2f> path_pos;
	std::vector<sf::Vector2f> path_pos1;

	std::vector<sf::Vector2f> path_neg;
	std::vector<sf::Vector2f> path_neg1;


	if (!m_world || !entity)
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