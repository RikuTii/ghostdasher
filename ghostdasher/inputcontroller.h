#pragma once
#include "localplayer.h"

class InputController
{
public:
	InputController() { m_localplayer = nullptr; };
	InputController(LocalPlayer* localPlayer) : m_localplayer(localPlayer)
	{

	}
	void ReadInput(float frameTime);

private:
	LocalPlayer* m_localplayer;
};