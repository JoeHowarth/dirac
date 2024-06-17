#include <flecs.h>
#include <fmt/core.h>

#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>
#include <random>

#include "utils/util.h"

sf::View initWindow(sf::RenderWindow& window);

int main() {
    auto      window = sf::RenderWindow{{1920u, 1080u}, "Base Template"};
    sf::View  view   = initWindow(window);
    sf::Clock frameClock;

    flecs::world ecs;

    ecs.set<flecs::Rest>({});

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

        // ensure that the rest system is run (and any user defined systems)
        ecs.progress(deltaTime.asSeconds());

        // example usage of debugDrawer and textDrawer
        debugDrawer.lineStrip({{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 0}});
        textDrawer.draw({.pos = {0, 0}, .size = 44, .color = sf::Color::White}, "Hello BOb");

        textDrawer.display(window);
        debugDrawer.display(window);
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