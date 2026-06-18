#pragma once

#include <SFML/System/Vector2.hpp>
#include <map>
#include <memory>
#include "Weapon.hpp"

class Map;

class Player {
public:
    Player();

    void resetForLevel(sf::Vector2f start);
    void resetAfterDeath();
    void update(float dt, const Map& map);
    void move(sf::Vector2f wishDir, float dt, const Map& map);
    void rotate(float angleRadians);
    void takeDamage(float amount);
    void heal(float amount);
    void addShield(float amount);

    void giveWeapon(WeaponType type, float ammo);
    bool hasWeapon(WeaponType type) const;
    bool switchWeapon(WeaponType type);
    Weapon& currentWeapon();
    const Weapon& currentWeapon() const;
    WeaponType currentWeaponType() const { return m_currentWeapon; }

    sf::Vector2f position() const { return m_position; }
    void setPosition(sf::Vector2f p) { m_position = p; }
    sf::Vector2f direction() const { return m_direction; }
    sf::Vector2f cameraPlane() const { return m_plane; }
    float hp() const { return m_hp; }
    float shield() const { return m_shield; }
    float maxHp() const { return m_maxHp; }
    float maxShield() const { return m_maxShield; }
    float radius() const { return m_radius; }
    bool isDead() const { return m_hp <= 0.f; }

private:
    sf::Vector2f m_position{1.5f, 1.5f};
    sf::Vector2f m_start{1.5f, 1.5f};
    sf::Vector2f m_direction{1.f, 0.f};
    sf::Vector2f m_plane{0.f, 0.66f};

    float m_hp = 50.f;
    float m_shield = 150.f;
    float m_maxHp = 50.f;
    float m_maxShield = 150.f;
    float m_speed = 3.f;
    float m_radius = 0.35f;
    float m_timeSinceDamage = 99.f;

    std::map<WeaponType, std::unique_ptr<Weapon>> m_weapons;
    WeaponType m_currentWeapon = WeaponType::Rifle;
};
