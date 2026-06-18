#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

class Game;

enum class ProjectileOwner {
    Player,
    Enemy
};

class Projectile {
public:
    Projectile(sf::Vector2f pos, sf::Vector2f dir, float speed, float damage, ProjectileOwner owner, sf::Color color, float radius = 0.12f);

    void update(Game& game, float dt);

    sf::Vector2f position() const { return m_position; }
    sf::Color color() const { return m_color; }
    bool alive() const { return m_alive; }
    ProjectileOwner owner() const { return m_owner; }
    float radius() const { return m_radius; }
    void kill() { m_alive = false; }

private:
    sf::Vector2f m_position;
    sf::Vector2f m_direction;
    float m_speed;
    float m_damage;
    ProjectileOwner m_owner;
    sf::Color m_color;
    float m_radius;
    bool m_alive = true;
};
