#pragma once

#include "Weapon.hpp"

class Railgun : public Weapon {
public:
    Railgun();
    WeaponType type() const override { return WeaponType::Railgun; }
    std::string name() const override { return "Railgun"; }
    WeaponAction update(const WeaponInput& input, float dt, Player& owner) override;
    void addAmmo(float amount) override;
    void resetForRespawn() override;
    int primaryAmmo() const override { return m_shells; }
    float charge01() const override;

private:
    int m_shells = 0;
    bool m_charging = false;
    float m_charge = 0.f;
};
