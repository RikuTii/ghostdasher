#include "inputcontroller.h"



void InputController::ReadInput(float frameTime)
{
	sf::Vector2f velocity;
	velocity.x = 0.0f;
	velocity.y = 0.0f;

	const float moveSpeed = m_localplayer->GetMovementSpeed();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		velocity.x = -moveSpeed;
		m_localplayer->SetFacing(Left);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		velocity.y = -moveSpeed;
		m_localplayer->SetFacing(Up);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		velocity.y = moveSpeed;
		m_localplayer->SetFacing(Down);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		velocity.x = moveSpeed;
		m_localplayer->SetFacing(Right);
	}

	if (velocity.x > 0.0f || velocity.x < 0.0f)
	{
		if (velocity.y > 0.0f || velocity.y < 0.0f)
		{
			velocity.y /= 2.0f;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		m_localplayer->DoAttack();
	}

	static bool wasDashPressed = false;

	if (!wasDashPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		m_localplayer->DoDash();

		wasDashPressed = true;
	}
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		wasDashPressed = false;
	}

	m_localplayer->SetGoalVelocity(velocity);
}