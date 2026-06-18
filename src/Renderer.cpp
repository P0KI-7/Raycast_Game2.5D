#include "Renderer.hpp"

#include <algorithm>
#include <cmath>

#include "Enemy.hpp"
#include "Pickup.hpp"
#include "Player.hpp"
#include "Projectile.hpp"
#include "Utilities.hpp"

Renderer::Renderer() = default;

void Renderer::draw(sf::RenderWindow& window, const Map& map,
                    const Player& player, const std::vector<Enemy*>& enemies,
                    const std::vector<Pickup>& pickups,
                    const std::vector<Projectile>& projectiles,
                    const std::vector<LineVisual>& lines,
                    const std::vector<ExplosionVisual>& explosions) {
  drawRaycastWalls(window, map, player);

  std::vector<Billboard> sprites;
  for (const auto& pickup : pickups) {
    if (!pickup.active()) continue;
    Billboard b;
    b.pos = pickup.position();
    b.color = pickup.color();
    b.radius = 0.25f;
    b.height = 0.6f;
    b.distSq = util::lengthSquared(
        {b.pos.x - player.position().x, b.pos.y - player.position().y});
    sprites.push_back(b);
  }
  for (const auto* enemy : enemies) {
    if (!enemy || enemy->dead()) continue;
    Billboard b;
    b.pos = enemy->position();
    b.color = enemy->color();
    b.radius = enemy->radius();
    b.height = enemy->type() == EnemyType::Brute ? 1.35f : 1.f;
    b.distSq = util::lengthSquared(
        {b.pos.x - player.position().x, b.pos.y - player.position().y});
    sprites.push_back(b);
  }
  for (const auto& projectile : projectiles) {
    if (!projectile.alive()) continue;
    Billboard b;
    b.pos = projectile.position();
    b.color = projectile.color();
    b.radius = projectile.radius();
    b.height = 0.35f;
    b.distSq = util::lengthSquared(
        {b.pos.x - player.position().x, b.pos.y - player.position().y});
    sprites.push_back(b);
  }
  for (const auto& explosion : explosions) {
    Billboard b;
    b.pos = explosion.pos;
    b.color = explosion.color;
    b.radius = explosion.radius;
    b.height = explosion.radius * 2.f;
    b.distSq = util::lengthSquared(
        {b.pos.x - player.position().x, b.pos.y - player.position().y});
    sprites.push_back(b);
  }

  drawBillboards(window, player, sprites);

  for (const auto& line : lines) {
    drawWorldLine(window, player, line.a, line.b, line.color, line.thickness);
  }
}

void Renderer::drawRaycastWalls(sf::RenderWindow& window, const Map& map,
                                const Player& player) {
  const sf::Vector2u size = window.getSize();
  const int w = static_cast<int>(size.x);
  const int h = static_cast<int>(size.y);
  m_zBuffer.assign(w, 1000.f);

  sf::RectangleShape ceiling(
      {static_cast<float>(w), static_cast<float>(h) / 2.f});
  ceiling.setFillColor(sf::Color(40, 45, 60));
  ceiling.setPosition(0.f, 0.f);
  window.draw(ceiling);

  sf::RectangleShape floor(
      {static_cast<float>(w), static_cast<float>(h) / 2.f});
  floor.setFillColor(sf::Color(45, 42, 38));
  floor.setPosition(0.f, static_cast<float>(h) / 2.f);
  window.draw(floor);

  for (int x = 0; x < w; ++x) {
    const float cameraX =
        2.f * static_cast<float>(x) / static_cast<float>(w) - 1.f;
    sf::Vector2f rayDir = player.direction() + player.cameraPlane() * cameraX;
    Map::RayHit hit = map.castRay(player.position(), rayDir, 80.f);
    float perpWallDist = std::max(0.0001f, hit.distance);
    m_zBuffer[x] = perpWallDist;

    int lineHeight = static_cast<int>(static_cast<float>(h) / perpWallDist);
    int drawStart = -lineHeight / 2 + h / 2;
    int drawEnd = lineHeight / 2 + h / 2;
    drawStart = std::max(0, drawStart);
    drawEnd = std::min(h - 1, drawEnd);

    sf::Color color =
        hit.side ? sf::Color(105, 105, 125) : sf::Color(135, 135, 160);
    if ((hit.tileX + hit.tileY) % 2 == 0) {
      color.r = static_cast<sf::Uint8>(std::min(255, color.r + 20));
      color.g = static_cast<sf::Uint8>(std::min(255, color.g + 20));
    }

    sf::RectangleShape stripe(
        {1.f, static_cast<float>(drawEnd - drawStart + 1)});
    stripe.setPosition(static_cast<float>(x), static_cast<float>(drawStart));
    stripe.setFillColor(color);
    window.draw(stripe);
  }
}

bool Renderer::projectPoint(const Player& player, sf::Vector2f point,
                            float& screenX, float& depth) const {
  const float screenW = static_cast<float>(m_zBuffer.size());
  sf::Vector2f sprite{point.x - player.position().x,
                      point.y - player.position().y};
  const sf::Vector2f dir = player.direction();
  const sf::Vector2f plane = player.cameraPlane();
  const float invDet = 1.f / (plane.x * dir.y - dir.x * plane.y);
  const float transformX = invDet * (dir.y * sprite.x - dir.x * sprite.y);
  const float transformY = invDet * (-plane.y * sprite.x + plane.x * sprite.y);
  if (transformY <= 0.05f) return false;
  screenX = (screenW / 2.f) * (1.f + transformX / transformY);
  depth = transformY;
  return true;
}

void Renderer::drawBillboards(sf::RenderWindow& window, const Player& player,
                              std::vector<Billboard>& sprites) {
  const sf::Vector2u size = window.getSize();
  const int w = static_cast<int>(size.x);
  const int h = static_cast<int>(size.y);
  std::sort(sprites.begin(), sprites.end(),
            [](const Billboard& a, const Billboard& b) {
              return a.distSq > b.distSq;
            });

  for (const auto& s : sprites) {
    float screenX = 0.f, depth = 0.f;
    if (!projectPoint(player, s.pos, screenX, depth)) continue;
    const int spriteHeight =
        std::abs(static_cast<int>(static_cast<float>(h) / depth * s.height));
    const int spriteWidth = std::abs(
        static_cast<int>(static_cast<float>(h) / depth * s.radius * 2.f));
    int drawStartY = -spriteHeight / 2 + h / 2;
    int drawEndY = spriteHeight / 2 + h / 2;
    int drawStartX = static_cast<int>(screenX) - spriteWidth / 2;
    int drawEndX = static_cast<int>(screenX) + spriteWidth / 2;
    drawStartY = std::max(0, drawStartY);
    drawEndY = std::min(h - 1, drawEndY);

    for (int stripe = drawStartX; stripe < drawEndX; ++stripe) {
      if (stripe < 0 || stripe >= w || depth >= m_zBuffer[stripe]) continue;
      sf::RectangleShape rect(
          {1.f, static_cast<float>(drawEndY - drawStartY + 1)});
      rect.setPosition(static_cast<float>(stripe),
                       static_cast<float>(drawStartY));
      rect.setFillColor(s.color);
      window.draw(rect);
    }
  }
}

void Renderer::drawWorldLine(sf::RenderWindow& window, const Player& player,
                             sf::Vector2f a, sf::Vector2f b, sf::Color color,
                             float thickness) {
  float ax = 0.f, ad = 0.f, bx = 0.f, bd = 0.f;
  if (!projectPoint(player, a, ax, ad) || !projectPoint(player, b, bx, bd))
    return;
  const float h = static_cast<float>(window.getSize().y);
  sf::Vector2f p1{ax, h * 0.5f};
  sf::Vector2f p2{bx, h * 0.5f};
  sf::Vector2f d{p2.x - p1.x, p2.y - p1.y};
  const float len = util::length(d);
  if (len < 1.f) return;
  sf::RectangleShape line({len, thickness});
  line.setOrigin(0.f, thickness / 2.f);
  line.setPosition(p1);
  line.setRotation(std::atan2(d.y, d.x) * 180.f / util::PI);
  line.setFillColor(color);
  window.draw(line);
}
