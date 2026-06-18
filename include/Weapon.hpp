#pragma once

#include <SFML/Graphics/Color.hpp>
#include <string>

class Player;

enum class WeaponType {
    Rifle = 0,
    Laser = 1,
    Railgun = 2,
    Plasma = 3
};

struct WeaponInput {
    bool triggerHeld = false;
    bool triggerPressed = false;
    bool triggerReleased = false;
    bool reloadPressed = false;
};

struct WeaponAction {
    enum class Kind {
        None,
        RifleShot,
        LaserTick,
        RailShot,
        PlasmaShot
    } kind = Kind::None;

    float damage = 0.f;
    float range = 40.f;
    bool piercing = false;
    sf::Color color = sf::Color::White;
};

class Weapon {
public:
    virtual ~Weapon() = default;

    virtual WeaponType type() const = 0;
    virtual std::string name() const = 0;
    virtual WeaponAction update(const WeaponInput& input, float dt, Player& owner) = 0;
    virtual void addAmmo(float amount) = 0;
    virtual void resetForRespawn() = 0;

    virtual bool owned() const { return m_owned; }
    virtual void setOwned(bool value) { m_owned = value; }
    virtual int primaryAmmo() const = 0;
    virtual int secondaryAmmo() const { return 0; }
    virtual float ammoSeconds() const { return 0.f; }
    virtual float heat01() const { return 0.f; }
    virtual float charge01() const { return 0.f; }
    virtual bool isReloading() const { return false; }
    virtual bool isLocked() const { return false; }

protected:
    bool m_owned = false;
};
