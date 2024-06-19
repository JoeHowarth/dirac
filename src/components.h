#pragma once

#include "utils/util.h"

NEWTYPE(Position, Vec2)
NEWTYPE(Velocity, Vec2)
NEWTYPE(Acceleration, Vec2)
NEWTYPE(Mass, int)
NEWTYPE(AnomalyMult, float)
NEWTYPE(Radius, float)

struct BoundingBox {
    Vec2 top, bot;

    bool inside(const Vec2& pt) const {
        return pt.x > top.x && pt.x < bot.x && pt.y > top.y && pt.y < bot.y;
    }

    bool intersects(const BoundingBox& other) const {
        return top.x < other.bot.x && bot.x > other.top.x &&
               top.y < other.bot.y && bot.y > other.top.y;
    }

    Vec2 center() const {
        return (top + bot) / 2.f;
    }

    Vec2 size() const {
        return bot - top;
    }

    void debug_draw() const {
        drawer.lineStrip(
            {top, Vec2(top.x, bot.y), bot, Vec2(bot.x, top.y), top},
            sf::Color::Red, 0
        );
    }
};

#define REGISTER_COMPONENT(TYPE) ecs.component<TYPE>(#TYPE)

void registerComponents(flecs::world& ecs) {
    REGISTER_COMPONENT(Position);
    REGISTER_COMPONENT(Velocity);
    REGISTER_COMPONENT(Acceleration);
    REGISTER_COMPONENT(Mass);
    REGISTER_COMPONENT(AnomalyMult);
    REGISTER_COMPONENT(Radius);
    REGISTER_COMPONENT(BoundingBox);
    REGISTER_COMPONENT(sf::Color);
}

/**** Relations ****/

struct CollidedWith {};

void registerRelations(flecs::world& ecs) {
    ecs.component<CollidedWith>().add(flecs::Symmetric);
}