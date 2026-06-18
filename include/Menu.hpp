#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "EnemySpawner.hpp"

class Menu {
 public:
  enum class Screen { Main, Difficulty, MapSelect, Death, Victory };

  enum class Action {
    None,
    Start,
    OpenDifficulty,
    Exit,
    BackToMain,
    SelectMap1,
    SelectMap2,
    SelectMap3,
    Easy,
    Medium,
    Hard,
    ContinueToMaps
  };

  void setFont(const sf::Font* font);
  Action handleEvent(const sf::Event& event, const sf::RenderWindow& window,
                     Screen screen);
  void draw(sf::RenderWindow& window, Screen screen, Difficulty difficulty,
            int score = 0, float deathTimer = 0.f);

 private:
  struct Button {
    sf::FloatRect rect;
    std::string text;
    Action action = Action::None;
  };

  std::vector<Button> buildButtons(sf::Vector2u size, Screen screen,
                                   Difficulty difficulty) const;
  void drawButton(sf::RenderWindow& window, const Button& button, bool hover);
  void drawTitle(sf::RenderWindow& window, const std::string& text, float y,
                 unsigned size);
  bool contains(const Button& b, sf::Vector2f p) const;

  const sf::Font* m_font = nullptr;
};
