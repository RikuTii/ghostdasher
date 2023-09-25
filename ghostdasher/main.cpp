#include "resourcemanager.h"
#include "entity.h"
#include "hostile.h"
#include "entitymanager.h"
#include "inputcontroller.h"
#include "pathfinder.h"
#include "globals.h"


std::unique_ptr<Globals> globals;

#include <tmxlite/Map.hpp>

#include "SFMLMapRender.h"

std::mutex path_mutex;

void UpdateHostilePathfinding()
{
	while (true)
	{
		path_mutex.lock();

		for (int i = 0; i < entityManager->GetHighestEntityIndex(); i++)
		{
			Entity* ent = entityManager->GetEntity(i);
			if (ent && ent->GetType() == EntityType::HostileEntity)
			{
				Hostile* hostile = static_cast<Hostile*>(ent);

				hostile->UpdatePath();

			}
		}

		path_mutex.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
	
	sf::RectangleShape obstacle;
	obstacle.setFillColor(sf::Color::Cyan);
	obstacle.setSize(sf::Vector2f(300, 300));
	obstacle.setPosition(sf::Vector2f(400, 400));



	sf::RectangleShape obstacle2;
	obstacle2.setFillColor(sf::Color::Yellow);
	obstacle2.setSize(sf::Vector2f(150, 300));
	obstacle2.setPosition(sf::Vector2f(200, 200));

	sf::RectangleShape obstacle3;
	obstacle3.setFillColor(sf::Color::Magenta);
	obstacle3.setSize(sf::Vector2f(300, 80));
	obstacle3.setPosition(sf::Vector2f(200, 600));

	resourceManager = std::make_unique<ResourceManager>();

	resourceManager->Loadresources();

	entityManager = std::make_unique<EntityManager>();


	World* world = entityManager->CreateWorld();

	LocalPlayer* localplayer = entityManager->CreateLocalPlayer();

	std::unique_ptr<InputController> inputcontroller = std::make_unique<InputController>(localplayer);


	pathFinder = std::make_unique<PathFinder>(world);


	Entity* newent = entityManager->AddEntity(std::make_unique<Hostile>());
	//entityManager->AddEntity(std::make_unique<Entity>());
	//entityManager->AddEntity(std::make_unique<Entity>());
	//entityManager->AddEntity(std::make_unique<Entity>());



	sf::CircleShape heart;
	heart.setOutlineColor(sf::Color::Cyan);
	heart.setRadius(20.0f);
	heart.setFillColor(sf::Color::Black);
	heart.setOutlineThickness(2);

	sf::Text fps;
	fps.setFillColor(sf::Color::White);
	fps.setCharacterSize(26);
	fps.setFont(resourceManager->GetPrimaryFont());
	fps.setPosition(0, 0);
	fps.setString("");


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

	gameView.zoom(2);
	sf::RectangleShape start;
	sf::VertexArray line;
	sf::VertexArray line2;
	sf::VertexArray line3;

	tmx::Map map;
	map.load("../assets/untitled.tmx");

	MapLayer layerZero(map, 0);
	MapLayer layerDepth(map, 2);

	const auto& layers = map.getLayers();
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


	newent->SetPosition(world->GetRandomSpawnPoint());
	((Hostile*)newent)->SetGoalPosition(newent->GetPosition());
	localplayer->SetPosition(world->GetRandomSpawnPoint());

	std::thread pathfinding_thread(UpdateHostilePathfinding);
	bool paused = false;

	while (window.isOpen())
	{
		sf::Time elapsed = clock.restart();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

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
		globals->interval_per_tick = 0.0078125f;
		globals->tick_count = static_cast<int>(globals->curtime / globals->interval_per_tick);



		window.setView(gameView);
		gameView.setCenter(localplayer->GetPosition());

		if (!paused)
		{
			entityManager->ProcessEntityLogic(elapsed.asSeconds());
			if (localplayer->IsAlive())
			{
				localplayer->Process(elapsed.asSeconds());

				inputcontroller->ReadInput(elapsed.asSeconds());
			}
		}


		for (int i = 0; i < entityManager->GetHighestEntityIndex(); i++)
		{
			Entity* ent = entityManager->GetEntity(i);
			if (ent && ent->GetType() == EntityType::HostileEntity)
			{
				Hostile* hostile = static_cast<Hostile*>(ent);

				sf::Vector2f dist = (localplayer->GetPosition() - hostile->GetPosition());

				float dist_len = std::sqrt(dist.x * dist.x + dist.y * dist.y);

				start.setPosition(localplayer->GetPosition());
				start.setSize(sf::Vector2f(1, dist_len));
				sf::Vector2f additional = rand() % 100 < 10 ? sf::Vector2f((float)(rand() % 500), (float)(rand() % 500)) : sf::Vector2f(0, 0);
				sf::Vector2f delta = (localplayer->GetPosition() - hostile->GetPosition());

				if (dist_len < 40.0f)
				{
					localplayer->TakeDamage(50, hostile->GetPosition());
				}

				float angle = atan2f(hostile->GetPosition().x - start.getPosition().x, hostile->GetPosition().y - start.getPosition().y) * 180 / 3.14f;
				start.setRotation(-angle);

				std::vector<sf::Vector2f> path;
				std::vector<sf::Vector2f> path2;
				std::vector<sf::Vector2f> path3;

				World* world = entityManager->GetWorld();

			//	hostile->SetPosition(sf::Vector2f(30, 600));
				hostile->GoToPosition(localplayer->GetPosition());

				sf::Vector2f startPos = hostile->GetPosition();

				const float step = 5.0f;
				sf::Vector2f local_pos = localplayer->GetPosition();
			
				if (world->IsPointVisible(local_pos,startPos))
				{
					start.setFillColor(sf::Color::Green);
				}
				else
				{
					start.setFillColor(sf::Color::Red);

				}



				if (localplayer->CheckSwordCollision(hostile->GetBounds()))
				{
					std::cout << "hit ent " << ent->GetPosition().x << std::endl;;
				//	hostile->SetPosition(hostile->GetPosition() - sf::Vector2f(delta.x * 2, delta.y * 2) / (elapsed.asSeconds() * 300));
					hostile->TakeDamage(50, localplayer->GetFacing());


				}
			}
		}


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
		fps.setString("FPS " + std::to_string(average_fps));

		last_frametime = elapsed.asSeconds();

		window.clear(sf::Color(43, 43, 43));
		world->Render(window);
		if (localplayer->IsAlive())
		{
			localplayer->Render(window);
		}

/*		window.draw(obstacle);
		window.draw(obstacle2);
		window.draw(obstacle3);*/
	//	window.draw(start);
		window.draw(layerZero);
		window.draw(layerDepth);



		entityManager->RenderEntities(window);

	//	window.draw(line);
	//	window.draw(line2);
	//	window.draw(line3);

		window.setView(view);
		window.draw(fps);
		if (paused)
		{
			window.draw(pausedText);
		}


		for (int i = 0; i < localplayer->GetHealth(); i += 50)
		{
			window.draw(heart);

			heart.setPosition(sf::Vector2f(i + 100, 30));
		}

		window.display();

		entityManager->DeleteMarkedEntities();
	}

	pathfinding_thread.detach();

	return 0;
}