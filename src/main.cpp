#include <flecs.h>
#include <fmt/core.h>

#include <SFML/Graphics.hpp>

#include "anomaloid.h"
#include "components.h"
#include "utils/util.h"

void spawnShip(flecs::world& ecs, Position pos) {
    BoundingBox box = {.top = {0, 0}, .bot = {0, 0}};

    int             i = 0;
    sf::ConvexShape gfx(5);
    for (Vec2 pt :
         {Vec2(0, -10), Vec2(8, 10), Vec2(2, 8), Vec2(-2, 8), Vec2(-8, 10)}) {
        gfx.setPoint(i++, pt);
        box.addPt(pt);
        fmt::println("Adding point: {}", pt);
        fmt::println("{}, {}", box.top, box.bot);
    }

    gfx.setFillColor(sf::Color::Green);
    gfx.setOutlineColor(sf::Color(150, 150, 150));
    gfx.setOutlineThickness(1);

    ecs.entity()
        .add<Ship>()
        .set(pos)
        .set(Velocity({0, 0}))
        .set(Acceleration({0, 0}))
        .set(Mass(5))
        .set(gfx)
        .set(box);
}

void spawnBullet(flecs::world& ecs, Position pos, Velocity vel) {
    int                i    = 0;
    Vec2               size = {4, 10};
    BoundingBox        box  = {.top = -size / 2.f, .bot = size / 2.f};
    sf::RectangleShape gfx(size);

    gfx.setOrigin(size.x / 2, size.y / 2);
    gfx.setFillColor(sf::Color::White);
    gfx.setOutlineColor(sf::Color::Black);
    gfx.setOutlineThickness(1);

    ecs.entity()
        .set(pos)
        .set(vel)
        .set(Acceleration({0, 0}))
        .set(Mass(1))
        .set(gfx)
        .set(box);
}

void renderBullet(flecs::world& ecs, sf::RenderTarget& window) {
    ecs.each([&](const Position& pos, sf::RectangleShape& gfx) {
        gfx.setPosition(pos.v);
        window.draw(gfx);
    });
}

void applyGravity(flecs::world& ecs, float dt) {
    const float G = 0.001;
    ecs.each([&](flecs::entity e, const Position& pos, Acceleration& acc,
                 const Mass& _mass) {
        ecs.each([&](flecs::entity other, const Position& otherPos,
                     const Mass& otherMass) {
            if (e == other) {
                return;
            }
            auto dist = magnitude(otherPos.v - pos.v);
            auto dir  = ((otherPos.v - pos.v) / dist);
            acc.v += dir * G * (float)(otherMass.v / (dist * dist));
        });
    });
}

void updatePhysicsMechanics(flecs::world& ecs, float dt) {
    ecs.each([&](flecs::entity e, Position& pos, Velocity& vel,
                 Acceleration& acc) {
        fmt::println("{}, entity: {}", acc, e);
        vel.v += acc.v * dt;
        pos.v += vel.v * dt;
        acc.v = {0, 0};
    });
}

void renderShip(flecs::world& ecs, sf::RenderTarget& window) {
    ecs.each([&](const Ship, const Position& pos, sf::ConvexShape& gfx) {
        gfx.setPosition(pos.v);
        window.draw(gfx);
    });
}

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

    spawnAnomaloids(ecs, 10);
    spawnShip(ecs, Position({0, 0}));

    spawnBullet(ecs, Position({0, 0}), Velocity({0.05, 0}));
    spawnBullet(ecs, Position({0, 0}), Velocity({0.00, 0}));

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            spawnBullet(
                ecs, Position({-100.f + i * 20, -100.f + j * 20}),
                Velocity({0.05, 0})
            );
        }
    }

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

        // applyGravity(ecs, deltaTime.asMilliseconds());
        updatePhysicsMechanics(ecs, deltaTime.asMilliseconds());

        collisionDetection(ecs);

        renderAnomaloids(ecs, window);
        renderBoundingBoxes(ecs, window);
        renderShip(ecs, window);
        renderBullet(ecs, window);

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