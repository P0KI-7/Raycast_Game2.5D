#pragma once

#include "Weapon.hpp"

class LaserGun : public Weapon {
public:
    LaserGun();
    WeaponType type() const override { return WeaponType::Laser; }
    std::string name() const override { return "Laser Gun"; }
    WeaponAction update(const WeaponInput& input, float dt, Player& owner) override;
    void addAmmo(float amount) override;
    void resetForRespawn() override;
    int primaryAmmo() const override { return static_cast<int>(m_ammoSeconds + 0.5f); }
    float ammoSeconds() const override { return m_ammoSeconds; }
    float heat01() const override;
    bool isLocked() const override { return m_lockTimer > 0.f; }

private:
    float m_ammoSeconds = 0.f;
    float m_heat = 0.f;
    float m_tickTimer = 0.f;
    float m_lockTimer = 0.f;
};
