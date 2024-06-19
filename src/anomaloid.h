#pragma once

#include "components.h"
#include "utils/util.h"

flecs::entity spawnAnomaloid(flecs::world& ecs, Mass mass, Position pos) {
    Radius      radius(mass.v * 5);
    BoundingBox boundingBox = {
        .top = Vec2(pos.v.x - radius.v, pos.v.y - radius.v),
        .bot = Vec2(pos.v.x + radius.v, pos.v.y + radius.v)
    };
    return ecs.entity()
        .set(AnomalyMult(randomFloat(1., 10)))
        .set(mass)
        .set(pos)
        .set(radius)
        .set(boundingBox);
}

void spawnAnomaloids2(flecs::world& ecs, int number) {
    spawnAnomaloid(ecs, Mass(10), Position({100, 100}));
    spawnAnomaloid(ecs, Mass(11), Position({100, 0}));
}

void collisionDetection(flecs::world& ecs) {
    DeferGuard g(ecs);
    ecs.each([&ecs](flecs::entity e1, const BoundingBox& box1) {
        ecs.each([&](flecs::entity e2, const BoundingBox& box2) {
            fmt::println("Checking {} and {}", e1, e2);
            if (e1 == e2) {
                return;
            }
            if (box1.intersects(box2)) {
                fmt::println("Collision detected! {} {}", e1, e2);
                e1.add<CollidedWith>(e2);
            }
        });
    });
}

void deleteCollided(flecs::world& ecs) {
    DeferGuard g(ecs);

    auto collidedWithQuery = ecs.query_builder<const Mass&>()
                                 .with<CollidedWith>(flecs::Wildcard)
                                 .with<AnomalyMult>()
                                 .cached()
                                 .build();
    collidedWithQuery.each([](flecs::iter& it, size_t index, const Mass& mass) {
        auto e     = it.entity(index);
        auto other = it.pair(1).second();
        fmt::println("Collision detected! {} {} (query)", e, other);
        if (!other.has<Mass>()) {
            fmt::println("[Error] No mass component on entity {}", other);
        }
        auto otherMass = *other.get<Mass>();
        fmt::println("Masses: {}, {}", mass, otherMass);
        if (mass.v > otherMass.v) {
            fmt::println(
                "[>] Setting color of entity {} to red, not {}", other, e
            );
            // other.set(sf::Color::Red);
            other.destruct();
        } else if (mass.v == otherMass.v) {
            flecs::entity max = std::max(e, other);
            // max.set(sf::Color::Red);
            fmt::println(
                "[=] Setting color of entity {} to red, not", max,
                e == max ? other : e
            );
            max.destruct();
        } else {
            ;
            fmt::println(
                "[<] Setting color of entity {} to red, not {}", e, other
            );
            // e.set(sf::Color::Red);
            e.destruct();
        }
    });
}

void spawnAnomaloids(flecs::world& ecs, int number) {
    std::exponential_distribution<float> d(1.5);
    for (int i = 0; i < number; ++i) {
        Mass     mass(d(gen) * 8);
        Position pos(randomVec2(-800, 800, -500, 500));
        spawnAnomaloid(ecs, mass, pos);
    }

    collisionDetection(ecs);
    deleteCollided(ecs);
}

void renderAnomaloids(flecs::world& ecs, sf::RenderTarget& window) {
    ecs.each([&](const flecs::entity e, const Position& pos,
                 const Radius& radius, const AnomalyMult& _) {
        sf::CircleShape circle(radius.v, 100);
        circle.setPosition(pos.v);
        circle.setOrigin(radius.v, radius.v);
        if (e.has<sf::Color>()) {
            circle.setFillColor(*e.get<sf::Color>());
        } else {
            circle.setFillColor(sf::Color::White);
        }
        window.draw(circle);
        textDrawer.draw({.pos = pos.v, .color = sf::Color::Black}, e);
        textDrawer.draw(
            {.pos = pos.v + Vec2(0, 20), .color = sf::Color::Black},
            *e.get<Mass>()
        );
    });
}