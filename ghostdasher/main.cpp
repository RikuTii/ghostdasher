#include "resourcemanager.h"
#include "entity.h"
#include "entitymanager.h"
#include "inputcontroller.h"

int main()
{

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(1200, 600), "Ghostdasher", sf::Style::Default, settings);
    window.setFramerateLimit(120);
    sf::Clock clock;

    srand((int)time(0));

    sf::RectangleShape base_map;
    base_map.setFillColor(sf::Color::Black);
    base_map.setSize(sf::Vector2f(1000, 1000));



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

    m_resourceManager = std::make_unique<ResourceManager>();

    m_resourceManager->Loadresources();

    std::unique_ptr<EntityManager> m_entityManager = std::make_unique<EntityManager>();


   World* world = m_entityManager->CreateWorld();

   LocalPlayer* localplayer = m_entityManager->CreateLocalPlayer();

   std::unique_ptr<InputController> inputcontroller = std::make_unique<InputController>(localplayer);


   m_entityManager->AddEntity(std::make_unique<Entity>());
   m_entityManager->AddEntity(std::make_unique<Entity>());
   m_entityManager->AddEntity(std::make_unique<Entity>());
   m_entityManager->AddEntity(std::make_unique<Entity>());



    sf::Text fps;
    fps.setFillColor(sf::Color::White);
    fps.setCharacterSize(26);
    fps.setFont(m_resourceManager->GetPrimaryFont());
    fps.setPosition(0, 0);
    fps.setString("");
    float shape_x = 0.0f;

    std::deque<float> m_frameSmaples;
    float last_frametime = 0.0f;

    sf::View view = window.getDefaultView();
    view.setSize(sf::Vector2f(view.getSize().x, view.getSize().y));

    sf::View gameView = window.getDefaultView();



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
                if (event.key.code == sf::Keyboard::F)
                {
                    m_entityManager->ResetAll();
                    m_entityManager->AddEntity(std::make_unique<Entity>());
                    m_entityManager->AddEntity(std::make_unique<Entity>());
                    m_entityManager->AddEntity(std::make_unique<Entity>());
                    m_entityManager->AddEntity(std::make_unique<Entity>());
                }
            }
        }

        window.setView(gameView);
        gameView.setCenter(localplayer->GetPosition());


        m_entityManager->ProcessEntityLogic(elapsed.asSeconds());
        localplayer->Process(elapsed.asSeconds());



        inputcontroller->ReadInput(elapsed.asSeconds());

        for (int i = 0; i < m_entityManager->GetHighestEntityIndex(); i++)
        {
            Entity* ent = m_entityManager->GetEntity(i);
            if (ent)
            {
               sf::Vector2f dist = (localplayer->GetPosition() - ent->GetPosition());

                float dist_len = std::sqrt(dist.x * dist.x + dist.y * dist.y);
                if (localplayer->CheckSwordCollision(ent->GetBounds()))
                {
                    ent->Delete();
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

        if (shape_x > window.getSize().x)
        {
            shape_x = 0.0f;
        }

        last_frametime = elapsed.asSeconds();
      
        window.clear(sf::Color(43,43,43));
        world->Render(window);
        localplayer->Render(window);

        window.draw(obstacle);
        window.draw(obstacle2);
        window.draw(obstacle3);

        m_entityManager->RenderEntities(window);
        window.setView(view);
        window.draw(fps);
        window.display();

        m_entityManager->DeleteMarkedEntities();
    }

	return 0;
}