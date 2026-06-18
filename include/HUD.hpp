#pragma once

#include <SFML/Graphics.hpp>

#include "EnemySpawner.hpp"

class Player;

class HUD {
 public:
  HUD();
  void setFont(const sf::Font* font);
  void draw(sf::RenderWindow& window, const Player& player,
            const EnemySpawner& spawner, int score);

 private:
  void drawBar(sf::RenderWindow& window, sf::Vector2f pos, sf::Vector2f size,
               float value01, sf::Color color, sf::Color back);
  void drawText(sf::RenderWindow& window, const std::string& str,
                sf::Vector2f pos, unsigned size = 20);
  const sf::Font* m_font = nullptr;
};
