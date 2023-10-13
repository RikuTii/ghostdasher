#include "resourcemanager.h"
#include "entity.h"
#include "hostile.h"
#include "firinghostile.h"
#include "entitymanager.h"
#include "inputcontroller.h"
#include "pathfinder.h"
#include "globals.h"
#include <tmxlite/Map.hpp>
#include "SFMLMapRender.h"
#include "crate.h"
#include "doorbutton.h"
#include "door.h"
#include "scaredhostile.h"

constexpr float TICK_INTERVAL = 0.0078125f;

std::unique_ptr<Globals> globals;



std::mutex path_mutex;

//Path finding thread for enemy pathfinding towards desired location. Expensive calculations so seperate thread
void UpdateHostilePathfinding()
{
	while (true)
	{
		path_mutex.lock();

		for (int i = 0; i < entityManager->GetHighestEntityIndex(); i++)
		{
			Entity* ent = entityManager->GetEntity(i);
			if (ent && (ent->GetType() == EntityType::HostileEntity || ent->GetType() == EntityType::ScaredHostileEntity))
			{
				Hostile* hostile = static_cast<Hostile*>(ent);
				hostile->UpdatePath();
			}
		}

		path_mutex.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

int main()
{

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderWindow window(sf::VideoMode(1200, 600), "Ghostdasher", sf::Style::Default, settings);
	window.setFramerateLimit(120);
	sf::Clock clock;
	sf::Clock currentClock;

	srand((int)time(0));

	globals = std::make_unique<Globals>();

	resourceManager = std::make_unique<ResourceManager>();

	resourceManager->Loadresources();

	entityManager = std::make_unique<EntityManager>();

	World* world = entityManager->CreateWorld();

	tmx::Map map;
	map.load("../assets/map1.tmx");

	MapLayer layerZero(map, 0);

	const auto& layers = map.getLayers();
	//Parse objects from map file for spawning, collisions etc
	for (const auto& layer : layers)
	{
		if (layer->getType() == tmx::Layer::Type::Object)
		{
			const auto& objects = layer->getLayerAs<tmx::ObjectGroup>().getObjects();
			for (const auto& object : objects)
			{
				if (object.getClass() == "spawnpoint")
				{
					world->AddSpawnPoint(sf::Vector2f(object.getPosition().x, object.getPosition().y));
				}
				else
				{
					tmx::FloatRect aabb = object.getAABB();
					world->AddUnwalkableSpace(sf::FloatRect(aabb.left, aabb.top, aabb.width, aabb.height));
				}
			}
		}
	}


	LocalPlayer* localplayer = entityManager->CreateLocalPlayer();

	std::unique_ptr<InputController> inputcontroller = std::make_unique<InputController>(localplayer);

	pathFinder = std::make_unique<PathFinder>(world);

	Hostile* newent = entityManager->AddEntity(std::make_unique<FiringHostile>());
	entityManager->AddEntity(std::make_unique<ScaredHostile>());

	Crate* test_crate = entityManager->AddEntity(std::make_unique<Crate>());
	Door* boss_door = entityManager->AddEntity(std::make_unique<Door>(sf::Vector2f(80,100)));

	std::vector<sf::Vector2f> m_button_positions = { sf::Vector2f(300,300),  sf::Vector2f(500,320),  sf::Vector2f(600,600) };
	for (auto it : m_button_positions)
	{
		entityManager->AddEntity(std::make_unique<DoorButton>(it));
	}

	sf::CircleShape heart;
	heart.setOutlineColor(sf::Color::Cyan);
	heart.setRadius(20.0f);
	heart.setFillColor(sf::Color::Black);
	heart.setOutlineThickness(2);

	sf::Text fps;
	fps.setFillColor(sf::Color::White);
	fps.setCharacterSize(26);
	fps.setFont(resourceManager->GetPrimaryFont());
	fps.setString("FPS: 1000");
	fps.setPosition(window.getSize().x - fps.getLocalBounds().width, 0);


	sf::Text pausedText;
	pausedText.setFillColor(sf::Color::White);
	pausedText.setCharacterSize(26);
	pausedText.setFont(resourceManager->GetPrimaryFont());
	pausedText.setPosition(window.getSize().x / 2, 0);
	pausedText.setString("PAUSED");


	std::deque<float> m_frameSmaples;
	float last_frametime = 0.0f;

	sf::View view = window.getDefaultView();
	view.setSize(sf::Vector2f(view.getSize().x, view.getSize().y));

	sf::View gameView = window.getDefaultView();
	gameView.zoom(1.2f);
	
	newent->SetPosition(world->GetRandomSpawnPoint());
	newent->SetGoalPosition(newent->GetPosition());
	localplayer->SetPosition(world->GetRandomSpawnPoint());
	test_crate->SetPosition(world->GetRandomSpawnPoint());

	std::thread pathfinding_thread(UpdateHostilePathfinding);
	bool paused = false;
	int m_last_tick = 0;
	while (window.isOpen())
	{
		sf::Time elapsed = clock.restart();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::LostFocus)
				paused = true;

			if (event.type == sf::Event::GainedFocus)
				paused = false;

			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					paused = !paused;
				}
			}

			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::F)
				{
					localplayer->Respawn();
					entityManager->ResetAll();
					for(int i = 0;i < 11;i++)
						entityManager->AddEntity(std::make_unique<Hostile>())->SetPosition(world->GetRandomSpawnPoint());;
				}
			}
		}


		
		globals->curtime = currentClock.getElapsedTime().asSeconds();
		globals->frametime = elapsed.asSeconds();
		globals->interval_per_tick = TICK_INTERVAL;
		globals->tick_count = static_cast<int>(globals->curtime / globals->interval_per_tick);


		window.setView(gameView);
		gameView.setCenter(localplayer->GetPosition());

		world->UpdateView(gameView.getCenter(), gameView.getSize());

		if (!paused)
		{
			//Process all entity logic. Position, movement etc
		
			entityManager->ProcessEntityLogic(elapsed.asSeconds());
			if (localplayer->IsAlive())
			{
				localplayer->Process(elapsed.asSeconds());

				inputcontroller->ReadInput(elapsed.asSeconds());
			}

			m_last_tick = globals->tick_count;
		}


		//Check localplayer sword hits against enemies
		for (int i = 0; i < entityManager->GetHighestEntityIndex(); i++)
		{
			Entity* ent = entityManager->GetEntity(i);
			if (ent && ent->GetType() == EntityType::HostileEntity)
			{
				Hostile* hostile = static_cast<Hostile*>(ent);

				hostile->GoToPosition();

				if (localplayer->CheckSwordCollision(hostile->GetBounds()))
				{
					hostile->TakeDamage(50, localplayer->GetFacing());
				}
			}
		}

		for (const auto& it : entityManager->GetEntitiesByType<ScaredHostile*>())
		{
			if (localplayer->CheckSwordCollision(it->GetBounds()))
			{
				it->TakeDamage(50, localplayer->GetFacing());
			}
		}

		//Check for destroyable crates
		for (const auto& it : entityManager->GetEntitiesByType<Crate*>())
		{
			if (localplayer->CheckSwordCollision(it->GetBounds()))
			{
				it->Delete();
			}
		}

		int pressed = 0;
		for (const auto& it : entityManager->GetEntitiesByType<DoorButton*>())
		{
			if (localplayer->GetBounds().intersects(it->GetBounds()))
			{
				it->SetPressed();
			}

			if (it->IsPressed())
			{
				pressed++;
			}
		}

		if (pressed > 2)
		{
			boss_door->SetOpen();
		}


		//Calculate average frames per second
		m_frameSmaples.push_back(elapsed.asSeconds());

		if (m_frameSmaples.size() > 10)
		{
			m_frameSmaples.pop_front();
		}

		float avg = 0.0f;
		for (auto it : m_frameSmaples)
		{
			avg += it;
		}

		const int average_fps = static_cast<int>(1.0f / (avg / m_frameSmaples.size()));
		fps.setString("FPS: " + std::to_string(average_fps));

		last_frametime = elapsed.asSeconds();


		//Clear previous window drawings
		window.clear(sf::Color(43, 43, 43));
		world->Render(window);
		if (localplayer->IsAlive())
		{
			localplayer->Render(window);
		}

		window.draw(layerZero);


		//Render each entiy with their specified render function
		entityManager->RenderEntities(window);

		window.setView(view);
		window.draw(fps);
		if (paused)
		{
			window.draw(pausedText);
		}

		//Draw current player health
		for (int i = 0; i < localplayer->GetHealth(); i += 50)
		{
			window.draw(heart);

			heart.setPosition(sf::Vector2f(i + 5, 10));
		}

		window.display();

		entityManager->DeleteMarkedEntities();
	}

	pathfinding_thread.detach();

	return 0;
}