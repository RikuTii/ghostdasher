#pragma once
#include "global-includes.h"
#include "entitymanager.h"
#include "world.h"
#include "hostile.h"
#include "scaredhostile.h"
#include "bouncingboss.h"
#include "inputcontroller.h"
#include "firinghostile.h"
#include "crate.h"
#include "doorbutton.h"
#include "door.h"
#include <tmxlite/Map.hpp>
#include "SFMLMapRender.h"

class Engine
{
public:
	Engine();
	Engine(World* _world, sf::RenderWindow* _window) : world(_world), window(_window)
	{
		Init();
	}

	void Init();
	void LoadMap(LocalPlayer* player, const std::string& name);
	void RenderUI(LocalPlayer* localplayer, sf::RenderWindow&);
	void RenderTileLayer(sf::RenderWindow&);
	void RenderBossLayer(sf::RenderWindow&);
	bool Run(LocalPlayer* player, InputController* input);
	void CreateDefaultEnemies();
	void ProcessEvent(const sf::Event&);
	void SetPaused(bool state)
	{
		m_paused = state;
	}

	sf::Vector2f GetLayerBounds()
	{
		if (tile_layer.get())
		{
			return sf::Vector2f(tile_layer.get()->getGlobalBounds().width, tile_layer.get()->getGlobalBounds().height);
		}

		return sf::Vector2f();
	}

	enum MenuOption
	{
		PlayButton,
		QuitButton
	};
private:
	World* world;
	sf::RenderWindow* window;
	std::unique_ptr<sf::Text> m_fps_text;
	sf::Clock current_clock;
	sf::Clock clock;
	std::unique_ptr<sf::Text> m_paused_text;
	std::unique_ptr<sf::RectangleShape> m_menu_background;
	std::unique_ptr<sf::Text> m_menu_title;
	std::list<std::unique_ptr<sf::Text>> m_menu_options;

	std::unique_ptr<MapLayer> tile_layer;
	std::unique_ptr<MapLayer> boss_layer;
	std::unique_ptr<MapLayer> boss_cover_layer;
	std::string m_map_name;


	std::deque<float> m_framesamples;
	bool m_paused;
	bool m_in_game;
};

extern std::unique_ptr<Engine> engine;