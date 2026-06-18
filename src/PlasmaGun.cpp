#include "PlasmaGun.hpp"
#include "Player.hpp"
#include <algorithm>

PlasmaGun::PlasmaGun() { m_owned = false; }

WeaponAction PlasmaGun::update(const WeaponInput& input, float dt, Player&) {
    if (m_cooldown > 0.f) m_cooldown -= dt;
    if (input.triggerHeld && m_cooldown <= 0.f && m_charges > 0) {
        --m_charges;
        m_cooldown = 0.25f;
        WeaponAction a;
        a.kind = WeaponAction::Kind::PlasmaShot;
        a.damage = 20.f;
        a.range = 40.f;
        a.color = sf::Color(40, 255, 80);
        return a;
    }
    return {};
}

void PlasmaGun::addAmmo(float amount) {
    m_owned = true;
    m_charges += static_cast<int>(amount);
}

void PlasmaGun::resetForRespawn() {
    m_owned = false;
    m_charges = 0;
    m_cooldown = 0.f;
}
