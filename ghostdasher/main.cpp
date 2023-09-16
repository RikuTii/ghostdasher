#include "resourcemanager.h"
#include "entity.h"
#include "entitymanager.h"

int main()
{

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(1200, 600), "Ghostdasher", sf::Style::Default, settings);
    window.setFramerateLimit(120);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Red);
    sf::Clock clock;
    sf::Font font;
    srand(time(0));

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

   Entity* player_entity =  m_entityManager->AddEntity(std::make_unique<Entity>());

    sf::Text fps;
    fps.setFillColor(sf::Color::White);
    fps.setCharacterSize(26);
    fps.setFont(m_resourceManager->GetPrimaryFont());
    fps.setPosition(0, 0);
    fps.setString("");
    float shape_x = 0.0f;

    std::deque<float> m_frameSmaples;
    float last_frametime = 0.0f;

    float vel_x = 0.0f;
    float vel_y = 0.0f;
    

    sf::View view = window.getDefaultView();
  
    view.setSize(sf::Vector2f(view.getSize().x, view.getSize().y));

    sf::View gameView = window.getDefaultView();

    sf::VertexArray line(sf::Lines, 2);

    line[0].color = sf::Color::Red;
    line[1].color = sf::Color::Red;

    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        window.setView(gameView);

        gameView.setCenter(player_entity->GetPosition());

        line[0].position = player_entity->GetPosition();

        m_entityManager->ProcessEntityLogic(elapsed.asSeconds());

        const float speed = 700.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
             vel_x = -speed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            vel_x = speed;

        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            vel_y = -speed;

        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            vel_y = speed;

        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
        {
            player_entity->ResetOrigin();
        }



        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            vel_x *= 10;

            vel_y *= 10;

        }
        player_entity->SetVelocity(sf::Vector2f(vel_x, vel_y));


        vel_y = 0.0f;

        vel_x = 0.0f;

        line[1].position = sf::Vector2f(0, 0);
        shape.setPosition(sf::Vector2f(shape_x, 0.0f));

        shape_x += elapsed.asMicroseconds() * 0.001f;

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
        //window.draw(line);
        window.draw(obstacle);
        window.draw(obstacle2);
        window.draw(obstacle3);

        window.draw(shape);
        m_entityManager->RenderEntities(window);

        window.setView(view);
  

        window.draw(fps);

        window.display();
    }

	return 0;
}