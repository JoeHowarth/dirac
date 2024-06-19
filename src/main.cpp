#include <flecs.h>
#include <fmt/core.h>

#include <SFML/Graphics.hpp>

#include "anomaloid.h"
#include "components.h"
#include "utils/util.h"

void renderBoundingBoxes(flecs::world& ecs, sf::RenderTarget& window) {
    ecs.each([&](const BoundingBox& box) { box.debug_draw(); });
}

sf::View initWindow(sf::RenderWindow& window);

int main() {
    auto      window = sf::RenderWindow{{1920u, 1080u}, "Base Template"};
    sf::View  view   = initWindow(window);
    sf::Clock frameClock;

    flecs::world ecs;
    registerComponents(ecs);
    registerRelations(ecs);
    ecs.set<flecs::Rest>({});

    spawnAnomaloids(ecs, 40);

    for (int frame = 0; window.isOpen(); ++frame) {
        sf::Time deltaTime = frameClock.restart();
        window.clear(sf::Color::Black);

        for (auto event = sf::Event{}; window.pollEvent(event);) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::Resized:
                    view.setSize(
                        static_cast<float>(event.size.width),
                        static_cast<float>(event.size.height)
                    );
                    window.setView(view);
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                    break;
                default:
                    break;
            }
        }

        collisionDetection(ecs);

        renderAnomaloids(ecs, window);
        renderBoundingBoxes(ecs, window);

        // ensure that the rest system is run (and any user defined systems)
        ecs.progress(deltaTime.asSeconds());

        textDrawer.display(window);
        drawer.display(window);
        window.display();
    }
}

sf::View initWindow(sf::RenderWindow& window) {
    window.setFramerateLimit(60);
    sf::Vector2u windowSize = window.getSize();
    sf::View     view;
    view.setCenter(0, 0);
    view.setSize(
        static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)
    );
    window.setView(view);
    return view;
}