#include "Game.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

#include "Collision.hpp"
#include "LaserGun.hpp"
#include "PlasmaGun.hpp"
#include "Railgun.hpp"
#include "Rifle.hpp"
#include "Utilities.hpp"

Game::Game(sf::RenderWindow& window) : m_window(window) {
  m_fontLoaded = util::loadDefaultFont(m_font);
  if (m_fontLoaded) {
    m_hud.setFont(&m_font);
    m_menu.setFont(&m_font);
  }
}

void Game::handleEvent(const sf::Event& event) {
  if (event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::Escape) {
    if (m_state == State::Playing) {
      m_window.setMouseCursorGrabbed(false);
      m_window.setMouseCursorVisible(true);
      m_state = State::Menu;
    }
  }

  Menu::Action action = Menu::Action::None;
  if (m_state != State::Playing && m_state != State::Death) {
    action = m_menu.handleEvent(event, m_window, menuScreenForState());
  }

  switch (action) {
    case Menu::Action::Start:
      m_state = State::MapSelect;
      break;
    case Menu::Action::OpenDifficulty:
      m_state = State::Difficulty;
      break;
    case Menu::Action::Exit:
      m_window.close();
      break;
    case Menu::Action::BackToMain:
      m_state = State::Menu;
      break;
    case Menu::Action::Easy:
      m_difficulty = Difficulty::Easy;
      break;
    case Menu::Action::Medium:
      m_difficulty = Difficulty::Medium;
      break;
    case Menu::Action::Hard:
      m_difficulty = Difficulty::Hard;
      break;
    case Menu::Action::SelectMap1:
      startLevel(1);
      break;
    case Menu::Action::SelectMap2:
      startLevel(2);
      break;
    case Menu::Action::SelectMap3:
      startLevel(3);
      break;
    case Menu::Action::ContinueToMaps:
      m_state = State::MapSelect;
      break;
    default:
      break;
  }
}

void Game::update(float dt) {
  if (m_state == State::Playing)
    updatePlaying(dt);
  else if (m_state == State::Death)
    updateDeath(dt);
}

void Game::draw() {
  if (m_state == State::Playing) {
    m_window.clear();
    auto raw = rawEnemyPointers();
    m_renderer.draw(m_window, m_map, m_player, raw, m_pickups, m_projectiles,
                    m_lines, m_explosions);
    m_hud.draw(m_window, m_player, m_spawner, m_score);
  } else {
    m_menu.draw(m_window, menuScreenForState(), m_difficulty, m_score,
                m_deathTimer);
  }
}

void Game::startLevel(int mapIndex) {
  m_currentMapIndex = mapIndex;
  std::string path = "assets/maps/map" + std::to_string(mapIndex) + ".txt";
  std::string error;
  if (!m_map.loadFromFile(path, &error)) {
    std::cerr << error << std::endl;
    return;
  }
  resetRuntimeObjects();
  m_player.resetForLevel(m_map.playerStart());
  for (const auto& marker : m_map.pickupMarkers()) {
    PickupType type = PickupType::Health;
    if (marker.type == 'K')
      type = PickupType::RifleAmmo;
    else if (marker.type == 'L')
      type = PickupType::Laser;
    else if (marker.type == 'R')
      type = PickupType::Railgun;
    else if (marker.type == 'G')
      type = PickupType::Plasma;
    m_pickups.emplace_back(type, marker.pos);
  }
  m_spawner.configure(m_difficulty, m_map.spawnPoints());
  m_score = 0;
  m_state = State::Playing;
  m_window.setMouseCursorGrabbed(true);
  m_window.setMouseCursorVisible(false);
  sf::Mouse::setPosition({static_cast<int>(m_window.getSize().x / 2),
                          static_cast<int>(m_window.getSize().y / 2)},
                         m_window);
}

void Game::resetRuntimeObjects() {
  m_pickups.clear();
  m_enemies.clear();
  m_projectiles.clear();
  m_lines.clear();
  m_explosions.clear();
  m_prevLeft = false;
  m_prevReload = false;
}

void Game::updatePlaying(float dt) {
  processInput(dt);
  m_player.update(dt, m_map);

  for (auto& pickup : m_pickups) {
    pickup.update(dt);
    pickup.tryCollect(m_player);
  }

  WeaponInput input;
  const bool left = sf::Mouse::isButtonPressed(sf::Mouse::Left);
  const bool reload = sf::Keyboard::isKeyPressed(sf::Keyboard::R);
  input.triggerHeld = left;
  input.triggerPressed = left && !m_prevLeft;
  input.triggerReleased = !left && m_prevLeft;
  input.reloadPressed = reload && !m_prevReload;
  m_prevLeft = left;
  m_prevReload = reload;

  WeaponAction action = m_player.currentWeapon().update(input, dt, m_player);
  processWeaponAction(action);

  updateProjectiles(dt);
  updateEnemies(dt);
  removeDeadEnemies();
  m_spawner.update(*this, dt);
  updateVisuals(dt);

  if (m_player.isDead()) {
    m_state = State::Death;
    m_deathTimer = 3.f;
    m_window.setMouseCursorGrabbed(false);
    m_window.setMouseCursorVisible(true);
  }
  if (m_spawner.complete()) {
    m_state = State::LevelComplete;
    m_window.setMouseCursorGrabbed(false);
    m_window.setMouseCursorVisible(true);
  }
}

void Game::updateDeath(float dt) {
  m_deathTimer -= dt;
  if (m_deathTimer <= 0.f) {
    m_player.resetAfterDeath();
    m_projectiles.clear();
    m_lines.clear();
    m_explosions.clear();
    m_state = State::Playing;
    m_window.setMouseCursorGrabbed(true);
    m_window.setMouseCursorVisible(false);
  }
}

void Game::processInput(float dt) {
  const sf::Vector2i center{static_cast<int>(m_window.getSize().x / 2),
                            static_cast<int>(m_window.getSize().y / 2)};
  const sf::Vector2i mouse = sf::Mouse::getPosition(m_window);
  const int dx = mouse.x - center.x;
  if (std::abs(dx) > 0) {
    m_player.rotate(static_cast<float>(dx) * 0.0025f);
    sf::Mouse::setPosition(center, m_window);
  }

  sf::Vector2f wish{0.f, 0.f};
  const sf::Vector2f dir = m_player.direction();
  const sf::Vector2f right{dir.y, -dir.x};
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) wish += dir;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) wish -= dir;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) wish -= right;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) wish += right;
  m_player.move(wish, dt, m_map);

  // Circular collision: the player cannot stand inside enemies.
  for (const auto& enemy : m_enemies) {
    if (enemy->dead()) continue;
    sf::Vector2f delta{m_player.position().x - enemy->position().x,
                       m_player.position().y - enemy->position().y};
    float len = util::length(delta);
    const float minDist = m_player.radius() + enemy->radius();
    if (len > 0.0001f && len < minDist) {
      sf::Vector2f target = enemy->position() + delta / len * (minDist + 0.01f);
      if (!m_map.isBlockedCircle(target, m_player.radius()))
        m_player.setPosition(target);
    }
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
    m_player.switchWeapon(WeaponType::Rifle);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
    m_player.switchWeapon(WeaponType::Laser);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
    m_player.switchWeapon(WeaponType::Railgun);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
    m_player.switchWeapon(WeaponType::Plasma);
}

void Game::processWeaponAction(const WeaponAction& action) {
  switch (action.kind) {
    case WeaponAction::Kind::RifleShot:
      hitscan(action.damage, action.range, false, action.color, false);
      break;
    case WeaponAction::Kind::LaserTick:
      hitscan(action.damage, action.range, true, action.color, false);
      break;
    case WeaponAction::Kind::RailShot:
      hitscan(action.damage, action.range, false, action.color, true);
      break;
    case WeaponAction::Kind::PlasmaShot:
      addProjectile(
          Projectile(m_player.position() + m_player.direction() * 0.45f,
                     m_player.direction(), 8.f, action.damage,
                     ProjectileOwner::Player, action.color, 0.13f));
      break;
    default:
      break;
  }
}

void Game::hitscan(float damage, float range, bool piercing, sf::Color color,
                   bool rail) {
  const sf::Vector2f origin = m_player.position();
  const sf::Vector2f dir = m_player.direction();
  Map::RayHit wall = m_map.castRay(origin, dir, range);
  float maxDist = wall.distance;
  sf::Vector2f endpoint = wall.point;

  Enemy* closest = nullptr;
  float closestT = maxDist;
  for (auto& enemy : m_enemies) {
    if (enemy->dead()) continue;
    float t = 0.f;
    if (collision::lineCircle(origin, dir, maxDist, enemy->position(),
                              enemy->radius(), &t)) {
      if (piercing) {
        enemy->takeDamage(damage);
      } else if (t < closestT) {
        closestT = t;
        closest = enemy.get();
      }
    }
  }

  if (!piercing && closest) {
    closest->takeDamage(damage);
    endpoint = closest->position();
  }

  if (rail) {
    addExplosion(endpoint, color, 1.1f, 0.22f);
    for (auto& enemy : m_enemies) {
      if (!enemy->dead() &&
          util::distance(enemy->position(), endpoint) <= 2.f) {
        enemy->takeDamage(100.f);
      }
    }
  }

  addLine(origin + dir * 0.2f, endpoint, color, rail ? 0.16f : 0.05f,
          rail ? 4.f : 2.f);
}

void Game::updateProjectiles(float dt) {
  for (auto& projectile : m_projectiles) projectile.update(*this, dt);
  m_projectiles.erase(
      std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                     [](const Projectile& p) { return !p.alive(); }),
      m_projectiles.end());
}

void Game::updateEnemies(float dt) {
  for (auto& enemy : m_enemies) enemy->update(*this, dt);
}

void Game::removeDeadEnemies() {
  auto it = m_enemies.begin();
  while (it != m_enemies.end()) {
    if ((*it)->dead()) {
      m_score += (*it)->scoreValue();
      m_spawner.onEnemyKilled();
      it = m_enemies.erase(it);
    } else
      ++it;
  }
}

void Game::updateVisuals(float dt) {
  for (auto& line : m_lines) line.timeLeft -= dt;
  m_lines.erase(
      std::remove_if(m_lines.begin(), m_lines.end(),
                     [](const LineVisual& l) { return l.timeLeft <= 0.f; }),
      m_lines.end());
  for (auto& e : m_explosions) e.timeLeft -= dt;
  m_explosions.erase(std::remove_if(m_explosions.begin(), m_explosions.end(),
                                    [](const ExplosionVisual& e) {
                                      return e.timeLeft <= 0.f;
                                    }),
                     m_explosions.end());
}

bool Game::hasLineOfSight(sf::Vector2f a, sf::Vector2f b) const {
  const sf::Vector2f dir = util::normalize({b.x - a.x, b.y - a.y});
  const float dist = util::distance(a, b);
  Map::RayHit hit = m_map.castRay(a, dir, dist);
  return !hit.hit || hit.distance >= dist - 0.1f;
}

bool Game::canEnemyMoveTo(sf::Vector2f pos, float radius,
                          const Enemy* self) const {
  if (m_map.isBlockedCircle(pos, radius)) return false;
  if (collision::circleCircle(pos, radius, m_player.position(),
                              m_player.radius()))
    return false;
  for (const auto& e : m_enemies) {
    if (e.get() == self || e->dead()) continue;
    if (collision::circleCircle(pos, radius, e->position(), e->radius()))
      return false;
  }
  return true;
}

void Game::damagePlayer(float amount) { m_player.takeDamage(amount); }
void Game::addProjectile(const Projectile& projectile) {
  m_projectiles.push_back(projectile);
}

void Game::spawnEnemy(EnemyType type, sf::Vector2f pos) {
  const auto s = m_spawner.settings();
  if (type == EnemyType::Grunt)
    m_enemies.push_back(std::make_unique<Grunt>(pos, s.enemyHpMultiplier,
                                                s.enemySpeedMultiplier));
  else if (type == EnemyType::Sniper)
    m_enemies.push_back(std::make_unique<Sniper>(pos, s.enemyHpMultiplier,
                                                 s.enemySpeedMultiplier));
  else
    m_enemies.push_back(std::make_unique<Brute>(pos, s.enemyHpMultiplier,
                                                s.enemySpeedMultiplier));
}

void Game::addLine(sf::Vector2f a, sf::Vector2f b, sf::Color color,
                   float duration, float thickness) {
  m_lines.push_back({a, b, color, duration, thickness});
}

void Game::addExplosion(sf::Vector2f pos, sf::Color color, float radius,
                        float duration) {
  m_explosions.push_back({pos, color, radius, duration});
}

std::vector<Enemy*> Game::rawEnemyPointers() const {
  std::vector<Enemy*> out;
  out.reserve(m_enemies.size());
  for (const auto& e : m_enemies) out.push_back(e.get());
  return out;
}

Menu::Screen Game::menuScreenForState() const {
  switch (m_state) {
    case State::Menu:
      return Menu::Screen::Main;
    case State::Difficulty:
      return Menu::Screen::Difficulty;
    case State::MapSelect:
      return Menu::Screen::MapSelect;
    case State::Death:
      return Menu::Screen::Death;
    case State::LevelComplete:
      return Menu::Screen::Victory;
    default:
      return Menu::Screen::Main;
  }
}
