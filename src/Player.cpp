#include "Player.hpp"
#include "Collision.hpp"
#include "LaserGun.hpp"
#include "Map.hpp"
#include "PlasmaGun.hpp"
#include "Railgun.hpp"
#include "Rifle.hpp"
#include "Utilities.hpp"
#include <algorithm>

Player::Player() {
    m_weapons[WeaponType::Rifle] = std::make_unique<Rifle>();
    m_weapons[WeaponType::Laser] = std::make_unique<LaserGun>();
    m_weapons[WeaponType::Railgun] = std::make_unique<Railgun>();
    m_weapons[WeaponType::Plasma] = std::make_unique<PlasmaGun>();
}

void Player::resetForLevel(sf::Vector2f start) {
    m_start = start;
    m_position = start;
    m_direction = {1.f, 0.f};
    m_plane = {0.f, 0.66f};
    m_hp = m_maxHp;
    m_shield = m_maxShield;
    m_timeSinceDamage = 99.f;
    for (auto& [_, weapon] : m_weapons) weapon->resetForRespawn();
    m_currentWeapon = WeaponType::Rifle;
}

void Player::resetAfterDeath() {
    m_position = m_start;
    m_direction = {1.f, 0.f};
    m_plane = {0.f, 0.66f};
    m_hp = m_maxHp;
    m_shield = 75.f;
    m_timeSinceDamage = 99.f;
    for (auto& [_, weapon] : m_weapons) weapon->resetForRespawn();
    m_currentWeapon = WeaponType::Rifle;
}

void Player::update(float dt, const Map&) {
    m_timeSinceDamage += dt;
    if (m_timeSinceDamage >= 7.f && m_shield < m_maxShield) {
        m_shield = std::min(m_maxShield, m_shield + 10.f * dt);
    }
}

void Player::move(sf::Vector2f wishDir, float dt, const Map& map) {
    if (util::lengthSquared(wishDir) <= 0.0001f) return;
    const sf::Vector2f delta = util::normalize(wishDir) * (m_speed * dt);
    m_position = collision::slideMove(m_position, delta, m_radius, map);
}

void Player::rotate(float angleRadians) {
    m_direction = util::normalize(util::rotate(m_direction, angleRadians));
    m_plane = util::rotate(m_plane, angleRadians);
}

void Player::takeDamage(float amount) {
    m_timeSinceDamage = 0.f;
    const float shieldDamage = std::min(m_shield, amount);
    m_shield -= shieldDamage;
    amount -= shieldDamage;
    if (amount > 0.f) m_hp = std::max(0.f, m_hp - amount);
}

void Player::heal(float amount) {
    m_hp = std::min(m_maxHp, m_hp + amount);
}

void Player::addShield(float amount) {
    m_shield = std::min(m_maxShield, m_shield + amount);
}

void Player::giveWeapon(WeaponType type, float ammo) {
    auto it = m_weapons.find(type);
    if (it != m_weapons.end()) it->second->addAmmo(ammo);
}

bool Player::hasWeapon(WeaponType type) const {
    auto it = m_weapons.find(type);
    return it != m_weapons.end() && it->second->owned();
}

bool Player::switchWeapon(WeaponType type) {
    if (!hasWeapon(type)) return false;
    m_currentWeapon = type;
    return true;
}

Weapon& Player::currentWeapon() { return *m_weapons.at(m_currentWeapon); }
const Weapon& Player::currentWeapon() const { return *m_weapons.at(m_currentWeapon); }
