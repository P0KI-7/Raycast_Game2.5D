#include "HUD.hpp"

#include <iomanip>
#include <sstream>

#include "Player.hpp"
#include "Utilities.hpp"
#include "Weapon.hpp"

HUD::HUD() = default;

void HUD::setFont(const sf::Font* font) { m_font = font; }

void HUD::drawBar(sf::RenderWindow& window, sf::Vector2f pos, sf::Vector2f size,
                  float value01, sf::Color color, sf::Color back) {
  sf::RectangleShape bg(size);
  bg.setPosition(pos);
  bg.setFillColor(back);
  window.draw(bg);
  sf::RectangleShape fg({size.x * util::clamp(value01, 0.f, 1.f), size.y});
  fg.setPosition(pos);
  fg.setFillColor(color);
  window.draw(fg);
  sf::RectangleShape outline(size);
  outline.setPosition(pos);
  outline.setFillColor(sf::Color::Transparent);
  outline.setOutlineColor(sf::Color(220, 220, 220));
  outline.setOutlineThickness(1.f);
  window.draw(outline);
}

void HUD::drawText(sf::RenderWindow& window, const std::string& str,
                   sf::Vector2f pos, unsigned size) {
  if (!m_font) return;
  sf::Text text;
  text.setFont(*m_font);
  text.setString(util::utf8(str));
  text.setCharacterSize(size);
  text.setFillColor(sf::Color::White);
  text.setPosition(pos);
  window.draw(text);
}

void HUD::draw(sf::RenderWindow& window, const Player& player,
               const EnemySpawner& spawner, int score) {
  const sf::Vector2u ws = window.getSize();
  const float centerX = static_cast<float>(ws.x) / 2.f;
  const float bottom = static_cast<float>(ws.y) - 70.f;

  drawBar(window, {centerX - 150.f, bottom}, {300.f, 14.f},
          player.shield() / player.maxShield(), sf::Color(60, 140, 255),
          sf::Color(20, 40, 70));
  drawBar(window, {centerX - 150.f, bottom + 20.f}, {300.f, 14.f},
          player.hp() / player.maxHp(), sf::Color(60, 230, 80),
          sf::Color(70, 20, 20));

  const Weapon& w = player.currentWeapon();
  std::ostringstream ammo;
  ammo << w.name() << "  ";
  if (player.currentWeaponType() == WeaponType::Rifle)
    ammo << w.primaryAmmo() << " / " << w.secondaryAmmo();
  else if (player.currentWeaponType() == WeaponType::Laser)
    ammo << static_cast<int>(w.ammoSeconds() + 0.5f) << " sec";
  else
    ammo << w.primaryAmmo();
  if (w.isReloading()) ammo << "  RELOAD";
  if (w.isLocked()) ammo << "  OVERHEAT";
  drawText(window, ammo.str(), {centerX + 170.f, bottom - 4.f}, 20);

  if (player.currentWeaponType() == WeaponType::Laser) {
    drawBar(window, {centerX + 170.f, bottom + 26.f}, {160.f, 10.f}, w.heat01(),
            sf::Color(255, 180, 40), sf::Color(60, 35, 20));
  }
  if (player.currentWeaponType() == WeaponType::Railgun && w.charge01() > 0.f) {
    drawBar(window, {centerX - 80.f, bottom - 22.f}, {160.f, 10.f},
            w.charge01(), sf::Color(80, 230, 255), sf::Color(20, 40, 50));
  }

  std::ostringstream left;
  left << "Ostalos mobov: " << spawner.remainingInWave() << " / "
       << spawner.totalInWave() << "\n";
  left << "Volna: " << spawner.currentWave() << " / " << spawner.totalWaves();
  drawText(window, left.str(), {20.f, 18.f}, 20);

  std::ostringstream right;
  right << "Score: " << score;
  drawText(window, right.str(), {static_cast<float>(ws.x) - 170.f, 18.f}, 20);

  sf::Vertex cross[] = {
      sf::Vertex({centerX - 8.f, static_cast<float>(ws.y) / 2.f},
                 sf::Color::White),
      sf::Vertex({centerX + 8.f, static_cast<float>(ws.y) / 2.f},
                 sf::Color::White),
      sf::Vertex({centerX, static_cast<float>(ws.y) / 2.f - 8.f},
                 sf::Color::White),
      sf::Vertex({centerX, static_cast<float>(ws.y) / 2.f + 8.f},
                 sf::Color::White)};
  window.draw(cross, 2, sf::Lines);
  window.draw(cross + 2, 2, sf::Lines);
}
