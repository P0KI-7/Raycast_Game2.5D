#pragma once

#include <SFML/Graphics.hpp>

#include "Game.hpp"

class App {
 public:
  App();
  void run();

 private:
  sf::RenderWindow m_window;
  Game m_game;
};
