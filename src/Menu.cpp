#include "Menu.hpp"

#include <sstream>

#include "Utilities.hpp"

void Menu::setFont(const sf::Font* font) { m_font = font; }

std::vector<Menu::Button> Menu::buildButtons(sf::Vector2u size, Screen screen,
                                             Difficulty difficulty) const {
  std::vector<Button> buttons;
  const float bw = 300.f;
  const float bh = 54.f;
  const float x = static_cast<float>(size.x) / 2.f - bw / 2.f;
  float y = 230.f;
  auto add = [&](const std::string& text, Action action) {
    buttons.push_back({sf::FloatRect(x, y, bw, bh), text, action});
    y += 72.f;
  };

  if (screen == Screen::Main) {
    add("Начать игру", Action::Start);
    add("Настройки", Action::OpenDifficulty);
    add("Выход", Action::Exit);
  } else if (screen == Screen::Difficulty) {
    add(difficulty == Difficulty::Easy ? "Лёгкая ✓" : "Лёгкая", Action::Easy);
    add(difficulty == Difficulty::Medium ? "Средняя ✓" : "Средняя",
        Action::Medium);
    add(difficulty == Difficulty::Hard ? "Сложная ✓" : "Сложная", Action::Hard);
    add("Назад", Action::BackToMain);
  } else if (screen == Screen::MapSelect) {
    add("Карта 1", Action::SelectMap1);
    add("Карта 2", Action::SelectMap2);
    add("Карта 3", Action::SelectMap3);
    add("Назад", Action::BackToMain);
  } else if (screen == Screen::Victory) {
    add("Продолжить", Action::ContinueToMaps);
  }
  return buttons;
}

bool Menu::contains(const Button& b, sf::Vector2f p) const {
  return b.rect.contains(p);
}

Menu::Action Menu::handleEvent(const sf::Event& event,
                               const sf::RenderWindow& window, Screen screen) {
  if (event.type != sf::Event::MouseButtonPressed ||
      event.mouseButton.button != sf::Mouse::Left)
    return Action::None;
  sf::Vector2f p =
      window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
  for (const auto& b :
       buildButtons(window.getSize(), screen, Difficulty::Medium)) {
    if (contains(b, p)) return b.action;
  }
  return Action::None;
}

void Menu::drawButton(sf::RenderWindow& window, const Button& button,
                      bool hover) {
  sf::RectangleShape rect({button.rect.width, button.rect.height});
  rect.setPosition(button.rect.left, button.rect.top);
  rect.setFillColor(hover ? sf::Color(85, 105, 150) : sf::Color(55, 65, 95));
  rect.setOutlineColor(sf::Color(210, 220, 255));
  rect.setOutlineThickness(2.f);
  window.draw(rect);

  if (m_font) {
    sf::Text t;
    t.setFont(*m_font);
    t.setString(util::utf8(button.text));
    t.setCharacterSize(24);
    t.setFillColor(sf::Color::White);
    sf::FloatRect bounds = t.getLocalBounds();
    t.setOrigin(bounds.left + bounds.width / 2.f,
                bounds.top + bounds.height / 2.f);
    t.setPosition(button.rect.left + button.rect.width / 2.f,
                  button.rect.top + button.rect.height / 2.f);
    window.draw(t);
  }
}

void Menu::drawTitle(sf::RenderWindow& window, const std::string& text, float y,
                     unsigned size) {
  if (!m_font) return;
  sf::Text t;
  t.setFont(*m_font);
  t.setString(util::utf8(text));
  t.setCharacterSize(size);
  t.setFillColor(sf::Color(245, 245, 255));
  sf::FloatRect b = t.getLocalBounds();
  t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
  t.setPosition(static_cast<float>(window.getSize().x) / 2.f, y);
  window.draw(t);
}

void Menu::draw(sf::RenderWindow& window, Screen screen, Difficulty difficulty,
                int score, float deathTimer) {
  window.clear(sf::Color(18, 22, 34));
  if (screen == Screen::Main)
    drawTitle(window, "2.5D SFML SHOOTER", 120.f, 44);
  else if (screen == Screen::Difficulty)
    drawTitle(window, "Сложность", 120.f, 44);
  else if (screen == Screen::MapSelect)
    drawTitle(window, "Выбор карты", 120.f, 44);
  else if (screen == Screen::Death) {
    drawTitle(window, "ВЫ УМЕРЛИ", 180.f, 54);
    std::ostringstream ss;
    ss << "Респавн через " << static_cast<int>(deathTimer + 1.f);
    drawTitle(window, ss.str(), 270.f, 30);
    return;
  } else if (screen == Screen::Victory) {
    drawTitle(window, "УРОВЕНЬ ПРОЙДЕН!", 150.f, 48);
    std::ostringstream ss;
    ss << "Score: " << score;
    drawTitle(window, ss.str(), 210.f, 30);
  }

  sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
  for (const auto& b : buildButtons(window.getSize(), screen, difficulty)) {
    drawButton(window, b, contains(b, mouse));
  }
}
