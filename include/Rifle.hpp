#pragma once

#include "Weapon.hpp"

class Rifle : public Weapon {
public:
    Rifle();
    WeaponType type() const override { return WeaponType::Rifle; }
    std::string name() const override { return "Rifle"; }
    WeaponAction update(const WeaponInput& input, float dt, Player& owner) override;
    void addAmmo(float amount) override;
    void resetForRespawn() override;
    int primaryAmmo() const override { return m_magazine; }
    int secondaryAmmo() const override { return m_reserve; }
    bool isReloading() const override { return m_reloadTimer > 0.f; }

private:
    int m_magazine = 60;
    int m_reserve = 300;
    float m_cooldown = 0.f;
    float m_reloadTimer = 0.f;
};
