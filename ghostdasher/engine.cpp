#include "engine.h"

std::unique_ptr<Engine> engine;

Engine::Engine()
{
	Init();
}

void Engine::Init()
{
	m_fps_text = std::make_unique<sf::Text>();
	m_fps_text->setFillColor(sf::Color::White);
	m_fps_text->setCharacterSize(26);
	m_fps_text->setFont(resourceManager->GetPrimaryFont());
	m_fps_text->setString("FPS: 1000");
	m_fps_text->setPosition(window->getSize().x - m_fps_text->getLocalBounds().width, 0);

	m_paused_text = std::make_unique<sf::Text>();
	m_paused_text->setFillColor(sf::Color::White);
	m_paused_text->setCharacterSize(26);
	m_paused_text->setFont(resourceManager->GetPrimaryFont());
	m_paused_text->setString("PAUSED");
	m_paused_text->setPosition(window->getSize().x / 2 - m_paused_text->getLocalBounds().width, 0);

	m_menu_background = std::make_unique<sf::RectangleShape>();
	m_menu_background->setSize(sf::Vector2f(window->getSize().x / 4, window->getSize().y));
	m_menu_background->setFillColor(sf::Color(30, 30, 30, 255));


	m_menu_title = std::make_unique<sf::Text>();
	m_menu_title->setFillColor(sf::Color::White);
	m_menu_title->setCharacterSize(36);
	m_menu_title->setFont(resourceManager->GetPrimaryFont());
	m_menu_title->setString("Ghostdasher");
	m_menu_title->setPosition(40, 50);

	const int option_gap = 30;

	std::unique_ptr<sf::Text> play_button = std::make_unique<sf::Text>();
	play_button->setFillColor(sf::Color::White);
	play_button->setCharacterSize(24);
	play_button->setFont(resourceManager->GetPrimaryFont());
	play_button->setString("Play");
	play_button->setPosition(40, 130);


	std::unique_ptr<sf::Text> restart_button = std::make_unique<sf::Text>();
	restart_button->setFillColor(sf::Color::White);
	restart_button->setCharacterSize(24);
	restart_button->setFont(resourceManager->GetPrimaryFont());
	restart_button->setString("Restart");
	restart_button->setPosition(40, 130 + option_gap);

	std::unique_ptr<sf::Text> quit_button = std::make_unique<sf::Text>();
	quit_button->setFillColor(sf::Color::White);
	quit_button->setCharacterSize(24);
	quit_button->setFont(resourceManager->GetPrimaryFont());
	quit_button->setString("Quit");
	quit_button->setPosition(40, 130 + option_gap *2);

	m_menu_options.push_back(std::move(play_button));
	m_menu_options.push_back(std::move(restart_button));

	m_menu_options.push_back(std::move(quit_button));

	m_in_game = true;
	m_paused = false;
}


void Engine::LoadMap(LocalPlayer* localplayer, const std::string& name)
{

	if (m_map_name.size())
	{
		world->Reset();
		entityManager->ResetAll();
		CreateDefaultEnemies();
		localplayer->Respawn();
	}

	tmx::Map map;
	if (map.load(name))
	{
		m_map_name = name;
		tile_layer = std::make_unique<MapLayer>(map, 0);
		boss_layer = std::make_unique<MapLayer>(map, 2);
		boss_cover_layer = std::make_unique<MapLayer>(map, 3);

		sf::FloatRect door_trigger;
		const auto& layers = map.getLayers();
		//Parse objects from map file for spawning, collisions etc
		for (const auto& layer : layers)
		{
			if (layer->getType() == tmx::Layer::Type::Object && layer->getName() == "objects")
			{
				const auto& objects = layer->getLayerAs<tmx::ObjectGroup>().getObjects();
				for (const auto& object : objects)
				{
					if (object.getClass() == "spawnpoint")
					{
						world->AddSpawnPoint(sf::Vector2f(object.getPosition().x, object.getPosition().y));
					}
					else if (object.getClass() == "door_button")
					{
						entityManager->AddEntity(std::make_unique<DoorButton>(sf::Vector2f(object.getPosition().x, object.getPosition().y)));
					}
					else if (object.getClass() == "local_spawn")
					{
						localplayer->SetPosition(sf::Vector2f(object.getPosition().x, object.getPosition().y));
					}
					else if (object.getClass() == "boss_spawn")
					{
						world->SetBossPosition(sf::Vector2f(object.getPosition().x, object.getPosition().y));
					}
					else if (object.getClass() == "boss_trigger")
					{
						door_trigger = sf::FloatRect(object.getAABB().left, object.getAABB().top, object.getAABB().width, object.getAABB().height);

					}
					else if (object.getClass() == "boss_door")
					{
						entityManager->AddEntity(std::make_unique<Door>(sf::Vector2f(object.getPosition().x, object.getPosition().y), sf::Vector2f(object.getAABB().width, object.getAABB().height)));
					}
					else
					{
						tmx::FloatRect aabb = object.getAABB();
						world->AddUnwalkableSpace(sf::FloatRect(aabb.left, aabb.top, aabb.width, aabb.height));
					}
				}
			}
			else if (layer->getType() == tmx::Layer::Type::Object && layer->getName() == "potential_objects")
			{
				const auto& objects = layer->getLayerAs<tmx::ObjectGroup>().getObjects();
				for (const auto& object : objects)
				{
					world->LoadPotentialObject(object);
				}
			}
		}

		for (const auto& it : entityManager->GetEntitiesByType<Door*>())
		{
			it->SetTrigger(door_trigger);
		}

		m_in_game = true;
	}
}

void Engine::RenderTileLayer(sf::RenderWindow& window)
{
	if (tile_layer)
	{
		window.draw(*tile_layer);
	}
}


void Engine::RenderBossLayer(sf::RenderWindow& window)
{
	if (boss_layer && boss_cover_layer)
	{
		if (world->IsBossFight())
		{
			window.draw(*boss_layer);
		}
		else
		{
			window.draw(*boss_cover_layer);
		}
	}
}

void Engine::CreateDefaultEnemies()
{
	entityManager->AddEntity(std::make_unique<FiringHostile>(world->GetRandomSpawnPoint()));
	entityManager->AddEntity(std::make_unique<ScaredHostile>(world->GetRandomSpawnPoint()));
	entityManager->AddEntity(std::make_unique<Hostile>())->SetPosition(world->GetRandomSpawnPoint());
}

void Engine::ProcessEvent(const sf::Event& event)
{
	if (event.type == sf::Event::LostFocus)
		m_paused = true;

	if (event.type == sf::Event::GainedFocus)
		m_paused = false;

	if (event.type == sf::Event::KeyReleased)
	{
		if (event.key.code == sf::Keyboard::Escape)
		{
			m_paused = !m_paused;
		}
	}
}

bool Engine::Run(LocalPlayer* localplayer, InputController* input)
{

	sf::Time elapsed = clock.restart();

	if (m_paused)
		return false;

	globals->curtime = current_clock.getElapsedTime().asSeconds();
	globals->frametime = elapsed.asSeconds();
	globals->interval_per_tick = TICK_INTERVAL;
	globals->tick_count = static_cast<int>(globals->curtime / globals->interval_per_tick);

	//Process all entity logic. Position, movement etc
	entityManager->ProcessEntityLogic(elapsed.asSeconds());
	if (localplayer->IsAlive())
	{
		input->ReadInput(elapsed.asSeconds());
	}

	localplayer->Process(elapsed.asSeconds());

	//Check localplayer sword hits against enemies
	for (int i = 0; i < entityManager->GetHighestEntityIndex(); i++)
	{
		Entity* ent = entityManager->GetEntity(i);
		if (ent && ent->GetType() == EntityType::HostileEntity)
		{
			Hostile* hostile = static_cast<Hostile*>(ent);

			if (localplayer->CheckSwordCollision(hostile->GetBounds()))
			{
				hostile->TakeDamage(50, localplayer->GetFacing());
			}

			sf::Vector2f delta = (localplayer->GetPosition() - hostile->GetPosition());

			float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

			if (dist < 30.0f)
			{
				localplayer->TakeDamage(50, hostile->GetPosition());
			}
		}
	}


	for (const auto& it : entityManager->GetEntitiesByType<ScaredHostile*>())
	{
		if (localplayer->CheckSwordCollision(it->GetBounds()))
		{
			it->TakeDamage(50, localplayer->GetFacing());
		}

		sf::Vector2f delta = (localplayer->GetPosition() - it->GetPosition());

		float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

		if (dist < 30.0f)
		{
			localplayer->TakeDamage(50, it->GetPosition());
		}
	}

	for (const auto& it : entityManager->GetEntitiesByType<BouncingBoss*>())
	{
		if (localplayer->CheckSwordCollision(it->GetBounds()))
		{
			it->TakeDamage(50, localplayer->GetPosition());
		}

		sf::Vector2f delta = (localplayer->GetPosition() - it->GetPosition());

		float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

		if (dist < 100.0f)
		{
			localplayer->TakeDamage(50.0f, it->GetPosition());
			it->SetBounceAngle(-it->GetBounceAngle());
		}
	}

	//Check for destroyable crates
	for (const auto& it : entityManager->GetEntitiesByType<Crate*>())
	{
		if (localplayer->CheckSwordCollision(it->GetBounds()))
		{
			it->Break();
		}

		if (it->IsBroken() && localplayer->GetBounds().intersects(it->GetBounds()))
		{
			if (localplayer->Heal(50))
			{
				it->Delete();
			}
		}
	}

	//Button entities to open door to leave the map
	bool has_pressed = true;
	for (const auto& it : entityManager->GetEntitiesByType<DoorButton*>())
	{
		if (localplayer->GetBounds().intersects(it->GetBounds()))
		{
			it->SetPressed();
		}

		if (!it->IsPressed())
		{
			has_pressed = false;
		}
	}

	if (has_pressed)
	{
		for (const auto& it : entityManager->GetEntitiesByType<Door*>())
		{
			it->SetOpen();
		}
	}


	//Calculate average frames per second
	m_framesamples.push_back(elapsed.asSeconds());

	if (m_framesamples.size() > 50)
	{
		m_framesamples.pop_front();
	}

	float avg = 0.0f;
	for (auto it : m_framesamples)
	{
		avg += it;
	}

	const int average_fps = static_cast<int>(1.0f / (avg / m_framesamples.size()));
	m_fps_text->setString("FPS: " + std::to_string(average_fps));

	return true;
}

void Engine::RenderUI(LocalPlayer* localplayer, sf::RenderWindow& render_window)
{
	render_window.draw(*m_fps_text);
	if (m_paused)
	{
		render_window.draw(*m_paused_text);
		render_window.draw(*m_menu_background);
		render_window.draw(*m_menu_title);

		sf::Vector2i mouse_pos = sf::Mouse::getPosition(render_window);
		sf::Vector2f real_pos = window->mapPixelToCoords(mouse_pos);
		sf::FloatRect mouse_bounds = sf::FloatRect(real_pos.x, real_pos.y, 5, 5);
		for (auto const& it : m_menu_options)
		{
			sf::Color color = it->getFillColor();

			if (m_in_game)
			{
				if (it->getString() == "Play")
				{
					it->setString("Continue");
				}
			}

			if (mouse_bounds.intersects(it->getGlobalBounds()))
			{
				it->setFillColor(sf::Color::Magenta);
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					if (it->getString() == "Quit")
					{
						window->close();
					}
					if (it->getString() == "Restart")
					{
						LoadMap(localplayer, m_map_name);
						m_paused = false;
					}
					if (it->getString() == "Continue")
					{
						m_paused = !m_paused;
					}
				}
			}
	
			render_window.draw(*it);

			it->setFillColor(sf::Color::White);
		}
	}
}