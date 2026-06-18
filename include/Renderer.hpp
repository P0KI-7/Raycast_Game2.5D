#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Map.hpp"

class Player;
class Enemy;
class Pickup;
class Projectile;

struct LineVisual {
  sf::Vector2f a;
  sf::Vector2f b;
  sf::Color color;
  float timeLeft = 0.f;
  float thickness = 2.f;
};

struct ExplosionVisual {
  sf::Vector2f pos;
  sf::Color color;
  float radius = 0.4f;
  float timeLeft = 0.f;
};

class Renderer {
 public:
  Renderer();
  void draw(sf::RenderWindow& window, const Map& map, const Player& player,
            const std::vector<Enemy*>& enemies,
            const std::vector<Pickup>& pickups,
            const std::vector<Projectile>& projectiles,
            const std::vector<LineVisual>& lines,
            const std::vector<ExplosionVisual>& explosions);

 private:
  struct Billboard {
    sf::Vector2f pos;
    sf::Color color;
    float radius = 0.35f;
    float height = 1.f;
    float distSq = 0.f;
  };

  void drawRaycastWalls(sf::RenderWindow& window, const Map& map,
                        const Player& player);
  void drawBillboards(sf::RenderWindow& window, const Player& player,
                      std::vector<Billboard>& sprites);
  void drawWorldLine(sf::RenderWindow& window, const Player& player,
                     sf::Vector2f a, sf::Vector2f b, sf::Color color,
                     float thickness);
  bool projectPoint(const Player& player, sf::Vector2f point, float& screenX,
                    float& depth) const;

  std::vector<float> m_zBuffer;
};
