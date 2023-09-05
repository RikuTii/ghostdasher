#include "resourcemanager.h"
#include "entity.h"
#include "entitymanager.h"

int main()
{

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1200, 600), "Ghostdasher",sf::Style::Default, settings);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Red);
    sf::Clock clock;
    sf::Font font;
    srand(time(0));
    m_resourceManager = std::make_unique<ResourceManager>();

    m_resourceManager->Loadresources();

    std::unique_ptr<EntityManager> m_entityManager = std::make_unique<EntityManager>();


    std::unique_ptr<Entity> my_entity = std::make_unique<Entity>();
    
    //std::unique_ptr<Entity> my_entity_2nd = std::make_unique<Entity>();

    std::shared_ptr<Entity> lol_ent =  m_entityManager->AddEntity(std::move(my_entity));
 //   m_entityManager->AddEntity(std::move(my_entity_2nd));

    sf::Text fps;
    fps.setFillColor(sf::Color::White);
    fps.setCharacterSize(26);
    fps.setFont(m_resourceManager->GetPrimaryFont());
    fps.setPosition(0, 0);
    fps.setString("hello");
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

        gameView.setCenter(lol_ent->GetPosition());

        line[0].position = lol_ent->GetPosition();

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
        lol_ent->SetVelocity(sf::Vector2f(vel_x, vel_y));


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
        window.draw(line);

        window.draw(shape);
        m_entityManager->RenderEntities(window);

        window.setView(view);
  

        window.draw(fps);

        window.display();
    }

	return 0;
}