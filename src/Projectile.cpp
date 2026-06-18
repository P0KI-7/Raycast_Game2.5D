#include "Projectile.hpp"
#include "Collision.hpp"
#include "Game.hpp"
#include "Utilities.hpp"

Projectile::Projectile(sf::Vector2f pos, sf::Vector2f dir, float speed, float damage, ProjectileOwner owner, sf::Color color, float radius)
    : m_position(pos), m_direction(util::normalize(dir)), m_speed(speed), m_damage(damage), m_owner(owner), m_color(color), m_radius(radius) {}

void Projectile::update(Game& game, float dt) {
    if (!m_alive) return;
    const sf::Vector2f next{m_position.x + m_direction.x * m_speed * dt, m_position.y + m_direction.y * m_speed * dt};
    if (game.map().isBlockedCircle(next, m_radius)) {
        m_position = next;
        m_alive = false;
        game.addExplosion(m_position, m_color, 0.35f, 0.18f);
        return;
    }
    m_position = next;

    if (m_owner == ProjectileOwner::Player) {
        for (const auto& enemy : game.enemies()) {
            if (!enemy->dead() && collision::circleCircle(m_position, m_radius, enemy->position(), enemy->radius())) {
                enemy->takeDamage(m_damage);
                m_alive = false;
                game.addExplosion(m_position, m_color, 0.35f, 0.18f);
                return;
            }
        }
    } else {
        Player& player = game.player();
        if (collision::circleCircle(m_position, m_radius, player.position(), player.radius())) {
            game.damagePlayer(m_damage);
            m_alive = false;
            game.addExplosion(m_position, m_color, 0.35f, 0.18f);
        }
    }
}
