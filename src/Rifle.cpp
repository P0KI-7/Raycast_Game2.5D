#include "Rifle.hpp"
#include "Player.hpp"
#include <algorithm>

Rifle::Rifle() { m_owned = true; }

WeaponAction Rifle::update(const WeaponInput& input, float dt, Player&) {
    if (m_cooldown > 0.f) m_cooldown -= dt;
    if (m_reloadTimer > 0.f) {
        m_reloadTimer -= dt;
        if (m_reloadTimer <= 0.f) {
            const int need = 60 - m_magazine;
            const int take = std::min(need, m_reserve);
            m_magazine += take;
            m_reserve -= take;
        }
        return {};
    }

    if (input.reloadPressed && m_magazine < 60 && m_reserve > 0) {
        m_reloadTimer = 1.5f;
        return {};
    }

    if (input.triggerHeld && m_cooldown <= 0.f && m_magazine > 0) {
        --m_magazine;
        m_cooldown = 0.1f;
        WeaponAction a;
        a.kind = WeaponAction::Kind::RifleShot;
        a.damage = 8.f;
        a.range = 40.f;
        a.color = sf::Color::White;
        return a;
    }
    return {};
}

void Rifle::addAmmo(float amount) {
    m_owned = true;
    m_reserve += static_cast<int>(amount);
}

void Rifle::resetForRespawn() {
    m_owned = true;
    m_magazine = 60;
    m_reserve = 300;
    m_cooldown = 0.f;
    m_reloadTimer = 0.f;
}
