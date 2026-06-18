#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "Brute.hpp"
#include "EnemySpawner.hpp"
#include "Grunt.hpp"
#include "HUD.hpp"
#include "Map.hpp"
#include "Menu.hpp"
#include "Pickup.hpp"
#include "Player.hpp"
#include "Projectile.hpp"
#include "Renderer.hpp"
#include "Sniper.hpp"

class Game {
 public:
  enum class State {
    Menu,
    Difficulty,
    MapSelect,
    Playing,
    Death,
    LevelComplete
  };

  explicit Game(sf::RenderWindow& window);

  void handleEvent(const sf::Event& event);
  void update(float dt);
  void draw();

  // API used by enemies and projectiles.
  const Map& map() const { return m_map; }
  Map& map() { return m_map; }
  Player& player() { return m_player; }
  const Player& player() const { return m_player; }
  const std::vector<std::unique_ptr<Enemy>>& enemies() const {
    return m_enemies;
  }
  DifficultySettings difficultySettings() const { return m_spawner.settings(); }

  bool hasLineOfSight(sf::Vector2f a, sf::Vector2f b) const;
  bool canEnemyMoveTo(sf::Vector2f pos, float radius, const Enemy* self) const;
  void damagePlayer(float amount);
  void addProjectile(const Projectile& projectile);
  void spawnEnemy(EnemyType type, sf::Vector2f pos);
  void addLine(sf::Vector2f a, sf::Vector2f b, sf::Color color, float duration,
               float thickness = 2.f);
  void addExplosion(sf::Vector2f pos, sf::Color color, float radius,
                    float duration);
  void addScore(int value) { m_score += value; }

 private:
  void startLevel(int mapIndex);
  void resetRuntimeObjects();
  void updatePlaying(float dt);
  void updateDeath(float dt);
  void processInput(float dt);
  void processWeaponAction(const WeaponAction& action);
  void hitscan(float damage, float range, bool piercing, sf::Color color,
               bool rail = false);
  void updateProjectiles(float dt);
  void updateEnemies(float dt);
  void removeDeadEnemies();
  void updateVisuals(float dt);
  std::vector<Enemy*> rawEnemyPointers() const;
  Menu::Screen menuScreenForState() const;

  sf::RenderWindow& m_window;
  State m_state = State::Menu;
  Difficulty m_difficulty = Difficulty::Medium;

  Map m_map;
  int m_currentMapIndex = 1;
  Player m_player;
  Renderer m_renderer;
  HUD m_hud;
  Menu m_menu;
  sf::Font m_font;
  bool m_fontLoaded = false;

  EnemySpawner m_spawner;
  std::vector<Pickup> m_pickups;
  std::vector<std::unique_ptr<Enemy>> m_enemies;
  std::vector<Projectile> m_projectiles;
  std::vector<LineVisual> m_lines;
  std::vector<ExplosionVisual> m_explosions;

  bool m_prevLeft = false;
  bool m_prevReload = false;
  int m_score = 0;
  float m_deathTimer = 3.f;
};
