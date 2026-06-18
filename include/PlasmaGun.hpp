#pragma once

#include "Weapon.hpp"

class PlasmaGun : public Weapon {
public:
    PlasmaGun();
    WeaponType type() const override { return WeaponType::Plasma; }
    std::string name() const override { return "Plasma Gun"; }
    WeaponAction update(const WeaponInput& input, float dt, Player& owner) override;
    void addAmmo(float amount) override;
    void resetForRespawn() override;
    int primaryAmmo() const override { return m_charges; }

private:
    int m_charges = 0;
    float m_cooldown = 0.f;
};
