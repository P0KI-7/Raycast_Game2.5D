#include "Pickup.hpp"
#include "Player.hpp"
#include "Utilities.hpp"

Pickup::Pickup(PickupType type, sf::Vector2f pos) : m_type(type), m_position(pos) {}

void Pickup::update(float dt) {
    if (!m_active) {
        m_respawnTimer -= dt;
        if (m_respawnTimer <= 0.f) m_active = true;
    }
}

bool Pickup::tryCollect(Player& player) {
    if (!m_active) return false;
    if (util::distance(player.position(), m_position) > 0.75f) return false;

    switch (m_type) {
        case PickupType::Health: player.heal(30.f); break;
        case PickupType::RifleAmmo: player.giveWeapon(WeaponType::Rifle, 300.f); break;
        case PickupType::Laser: player.giveWeapon(WeaponType::Laser, 30.f); break;
        case PickupType::Railgun: player.giveWeapon(WeaponType::Railgun, 5.f); break;
        case PickupType::Plasma: player.giveWeapon(WeaponType::Plasma, 200.f); break;
    }
    m_active = false;
    m_respawnTimer = 30.f;
    return true;
}

sf::Color Pickup::color() const {
    switch (m_type) {
        case PickupType::Health: return sf::Color(60, 240, 90);
        case PickupType::RifleAmmo: return sf::Color(230, 230, 230);
        case PickupType::Laser: return sf::Color(255, 40, 40);
        case PickupType::Railgun: return sf::Color(80, 230, 255);
        case PickupType::Plasma: return sf::Color(40, 255, 80);
    }
    return sf::Color::White;
}
