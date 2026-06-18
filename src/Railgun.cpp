#include "Railgun.hpp"
#include "Player.hpp"
#include "Utilities.hpp"
#include <algorithm>

Railgun::Railgun() { m_owned = false; }

WeaponAction Railgun::update(const WeaponInput& input, float dt, Player&) {
    if (input.triggerPressed && m_shells > 0) {
        m_charging = true;
        m_charge = 0.f;
    }
    if (m_charging && input.triggerHeld) {
        m_charge = std::min(4.f, m_charge + dt);
    }
    if (m_charging && input.triggerReleased) {
        const bool ready = m_charge >= 4.f && m_shells > 0;
        m_charging = false;
        m_charge = 0.f;
        if (ready) {
            --m_shells;
            WeaponAction a;
            a.kind = WeaponAction::Kind::RailShot;
            a.damage = 200.f;
            a.range = 45.f;
            a.color = sf::Color(80, 230, 255);
            return a;
        }
    }
    return {};
}

void Railgun::addAmmo(float amount) {
    m_owned = true;
    m_shells += static_cast<int>(amount);
}

void Railgun::resetForRespawn() {
    m_owned = false;
    m_shells = 0;
    m_charging = false;
    m_charge = 0.f;
}

float Railgun::charge01() const { return util::clamp(m_charge / 4.f, 0.f, 1.f); }
