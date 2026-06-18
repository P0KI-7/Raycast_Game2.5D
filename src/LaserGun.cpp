#include "LaserGun.hpp"
#include "Player.hpp"
#include <algorithm>

LaserGun::LaserGun() { m_owned = false; }

WeaponAction LaserGun::update(const WeaponInput& input, float dt, Player&) {
    if (m_lockTimer > 0.f) {
        m_lockTimer -= dt;
        if (m_lockTimer <= 2.f) m_heat = std::max(0.f, m_heat - 20.f * dt);
        return {};
    }

    bool firing = input.triggerHeld && m_ammoSeconds > 0.f && m_heat < 100.f;
    if (firing) {
        m_heat += 10.f * dt;
        m_ammoSeconds = std::max(0.f, m_ammoSeconds - dt);
        if (m_heat >= 100.f) {
            m_heat = 100.f;
            m_lockTimer = 4.f;
            return {};
        }
        m_tickTimer += dt;
        if (m_tickTimer >= 0.05f) {
            m_tickTimer = 0.f;
            WeaponAction a;
            a.kind = WeaponAction::Kind::LaserTick;
            a.damage = 0.75f;
            a.range = 40.f;
            a.piercing = true;
            a.color = sf::Color(255, 30, 30);
            return a;
        }
    } else {
        m_tickTimer = 0.f;
        m_heat = std::max(0.f, m_heat - 20.f * dt);
    }
    return {};
}

void LaserGun::addAmmo(float amount) {
    m_owned = true;
    m_ammoSeconds += amount;
}

void LaserGun::resetForRespawn() {
    m_owned = false;
    m_ammoSeconds = 0.f;
    m_heat = 0.f;
    m_tickTimer = 0.f;
    m_lockTimer = 0.f;
}

float LaserGun::heat01() const { return m_heat / 100.f; }
