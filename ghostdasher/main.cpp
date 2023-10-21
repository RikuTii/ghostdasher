#include "resourcemanager.h"
#include "entitymanager.h"
#include "inputcontroller.h"
#include "pathfinder.h"
#include "globals.h"
#include "entity.h"
#include <tmxlite/Map.hpp>
#include "SFMLMapRender.h"
#include "engine.h"


std::unique_ptr<Globals> globals;
std::unique_ptr<sf::RenderWindow> window;

std::mutex path_mutex;

//Path finding thread for enemy pathfinding towards desired location. Expensive calculations so seperate thread
void UpdateHostilePathfinding()
{
	while (true)
	{
		path_mutex.lock();

		if (entityManager)
		{
			for (int i = 0; i < entityManager->GetHighestEntityIndex(); i++)
			{
				Entity* ent = entityManager->GetEntity(i);
				if (ent && (ent->GetType() == EntityType::HostileEntity || ent->GetType() == EntityType::ScaredHostileEntity) && entityManager->GetWorld())
				{
					Hostile* hostile = static_cast<Hostile*>(ent);
					hostile->UpdatePath();
				}
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

	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(1200, 600), "Ghostdasher", sf::Style::Default, settings);
	window->setFramerateLimit(120);

	srand((int)time(0));

	globals = std::make_unique<Globals>();

	resourceManager = std::make_unique<ResourceManager>();

	resourceManager->Loadresources();

	entityManager = std::make_unique<EntityManager>();

	World* world = entityManager->CreateWorld();
	pathFinder = std::make_unique<PathFinder>(world);

	engine = std::make_unique<Engine>(world, window.get());

	LocalPlayer* localplayer = entityManager->CreateLocalPlayer();
	std::unique_ptr<InputController> inputcontroller = std::make_unique<InputController>(localplayer);

	engine->LoadMap(localplayer, "../assets/boss_map.tmx");

	world->SetBounds(engine->GetLayerBounds());

	engine->CreateDefaultEnemies();

	sf::View view = window->getDefaultView();

	sf::View gameView = window->getDefaultView();
	gameView.zoom(1.2f);

	std::thread pathfinding_thread(UpdateHostilePathfinding);

	float gameZoom = 1.0f;

	while (window->isOpen())
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();

			engine->ProcessEvent(event);

			if (event.type == sf::Event::MouseWheelScrolled)
			{
				auto data = event.mouseWheelScroll;
				if (data.delta > 0.0f)
				{
					gameZoom -= 0.1f;
					gameZoom = std::max(0.0f, gameZoom);
					gameView.zoom(gameZoom);
				}
				else
				{
					gameZoom += 0.1f;
					gameZoom = std::max(0.0f, gameZoom);
					gameView.zoom(gameZoom);
				}
			}
		}


		//Delete all entities which are marked for deletion
		entityManager->DeleteMarkedEntities();

		window->setView(gameView);
		gameView.setCenter(localplayer->GetPosition());

		world->UpdateView(gameView.getCenter(), gameView.getSize());

		//Update collisions for entities which can be removed from the world
		world->UpdateUnwalkableSpaces();

		//Main function to process  all entities and their logic
		engine->Run(localplayer, inputcontroller.get());

		//Clear previous window drawings
		window->clear(sf::Color(43, 43, 43));

		world->Render(*window);
		if (localplayer->IsAlive())
		{
			localplayer->Render(*window);
		}

		engine->RenderTileLayer(*window);
		world->RenderExtra(*window);
		engine->RenderBossLayer(*window);

		//Render each entity with their specified render function
		entityManager->RenderEntities(*window);

		//Restore view for rendering HUD and non world related objects
		window->setView(view);

		localplayer->RenderHud(*window);

		if (world->IsBossFight())
		{
			for (const auto& it : entityManager->GetEntitiesByType<BouncingBoss*>())
			{
				it->RenderHud(*window);
			}
		}

		engine->RenderUI(localplayer, *window);


		//Display all rendered items
		window->display();
	}

	pathfinding_thread.detach();

	return 0;
}