#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "Weapon.hpp"

class Player;

enum class PickupType {
    Health,
    RifleAmmo,
    Laser,
    Railgun,
    Plasma
};

class Pickup {
public:
    Pickup(PickupType type, sf::Vector2f pos);

    void update(float dt);
    bool tryCollect(Player& player);

    PickupType type() const { return m_type; }
    sf::Vector2f position() const { return m_position; }
    bool active() const { return m_active; }
    sf::Color color() const;

private:
    PickupType m_type;
    sf::Vector2f m_position;
    bool m_active = true;
    float m_respawnTimer = 0.f;
};
