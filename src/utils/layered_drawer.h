#pragma once

#include <SFML/Graphics.hpp>

#include "vectors.h"

struct Line : public sf::Drawable {
    sf::Vertex vertices[2];

    Line(const Vec2 start, const Vec2 end) {
        this->vertices[0] = start;
        this->vertices[1] = end;
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw((sf::Vertex*)vertices, 2, sf::Lines, states);
    }
};

struct LineStrip : public sf::Drawable {
    std::vector<sf::Vertex> vertices;

    LineStrip(const std::vector<Vec2>& points) {
        LineStrip(points, sf::Color::Red);
    }

    LineStrip(const std::vector<Vec2>& points, sf::Color color) {
        for (const auto& point : points) {
            this->vertices.push_back(sf::Vertex(point, color));
        }
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(vertices.data(), vertices.size(), sf::LineStrip, states);
    }
};

template <typename T, typename Func>
concept CallableWith = requires(Func f, T t) {
    { f(t) } -> std::convertible_to<Vec2>;
};

struct LayeredDrawer {
    std::vector<std::vector<std::unique_ptr<sf::Drawable>>> layers;
    const bool clearEveryFrame = false;

    LayeredDrawer(int numLayers = 1) : layers(numLayers) {}

    void draw(std::unique_ptr<sf::Drawable> drawable, int layer = 0) {
        this->layers[layer].push_back(std::move(drawable));
    }

    void line(const Vec2 start, const Vec2 end, int layer = 0) {
        this->layers[layer].push_back(std::make_unique<Line>(start, end));
    }

    void lineStrip(
        const std::vector<Vec2>& points,
        sf::Color                color = sf::Color::Red,
        int                      layer = 0
    ) {
        this->layers[layer].push_back(std::make_unique<LineStrip>(points, color)
        );
    }

    template <std::forward_iterator It, typename Func>
        requires CallableWith<
            typename std::iterator_traits<It>::value_type,
            Func>
    void lineStripMap(It begin, It end, Func toWorld) {
        std::vector<Vec2> path;
        std::transform(begin, end, std::back_inserter(path), toWorld);
        this->lineStrip(path, sf::Color::Red);
    }

    void point(const Vec2 point, int layer = 0) {
        std::unique_ptr<sf::CircleShape> circle =
            std::make_unique<sf::CircleShape>(2);
        circle->setPosition(point);
        circle->setFillColor(sf::Color::Red);
        this->layers[layer].push_back(
            std::unique_ptr<sf::Drawable>(std::move(circle))
        );
    }

    void clear(int layer) {
        this->layers[layer].clear();
    }

    void display(sf::RenderWindow& window) {
        for (auto& layer : this->layers) {
            for (const auto& drawable : layer) {
                window.draw(*drawable);
            }
            if (this->clearEveryFrame) {
                layer.clear();
            }
        }
    }
};