#pragma once

#include "components.h"
#include "utils/util.h"

void collisionDetection(flecs::world& ecs) {
    DeferGuard g(ecs);
    ecs.each([&ecs](flecs::entity e1, const BoundingBox& box1) {
        ecs.each([&](flecs::entity e2, const BoundingBox& box2) {
            // fmt::println("Checking {} and {}", e1, e2);
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

flecs::entity spawnAnomaloid(flecs::world& ecs, Mass mass, Position pos) {
    Radius      radius(mass.v);
    BoundingBox boundingBox = {
        .top = Vec2(pos.v.x - radius.v, pos.v.y - radius.v),
        .bot = Vec2(pos.v.x + radius.v, pos.v.y + radius.v)
    };

    sf::CircleShape circle(radius.v, 100);
    circle.setPosition(pos.v);
    circle.setOrigin(radius.v, radius.v);
    circle.setFillColor(sf::Color::White);
    circle.setOutlineColor(sf::Color(230, 230, 230));
    circle.setOutlineThickness(1);

    return ecs.entity()
        .set(AnomalyMult(randomFloat(1., 10)))
        .set(mass)
        .set(pos)
        .set(radius)
        .set(boundingBox)
        .set(std::move(circle));
}

void spawnAnomaloids(flecs::world& ecs, int number) {
    std::exponential_distribution<float> d(1.5);
    for (int i = 0; i < number; ++i) {
        Mass     mass(d(gen) * 40);
        Position pos(randomVec2(-800, 800, -500, 500));
        spawnAnomaloid(ecs, mass, pos);
    }

    collisionDetection(ecs);
    deleteCollided(ecs);
}

void renderAnomaloids(flecs::world& ecs, sf::RenderTarget& window) {
    ecs.each([&](const flecs::entity e, const Position& pos,
                 sf::CircleShape& circle, const AnomalyMult& _) {
        if (auto color = e.get<sf::Color>()) {
            circle.setFillColor(*color);
        } else {
            circle.setFillColor(sf::Color::White);
        }
        circle.setPosition(pos.v);

        window.draw(circle);

        textDrawer.draw({.pos = pos.v, .color = sf::Color::Black}, e);
        textDrawer.draw(
            {.pos = pos.v + Vec2(0, 20), .color = sf::Color::Black},
            *e.get<Mass>()
        );
    });
}

// void spawnAnomaloids2(flecs::world& ecs, int number) {
//     spawnAnomaloid(ecs, Mass(10), Position({100, 100}));
//     spawnAnomaloid(ecs, Mass(11), Position({100, 0}));
// }