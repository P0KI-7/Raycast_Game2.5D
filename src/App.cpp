#include "App.hpp"

App::App()
    : m_window(sf::VideoMode(1280, 720), "2.5D SFML Raycast Shooter",
               sf::Style::Close),
      m_game(m_window) {
  m_window.setFramerateLimit(120);
}

void App::run() {
  sf::Clock clock;
  const float fixedDt = 1.f / 60.f;
  float accumulator = 0.f;

  while (m_window.isOpen()) {
    sf::Event event{};
    while (m_window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) m_window.close();
      m_game.handleEvent(event);
    }

    accumulator += clock.restart().asSeconds();
    if (accumulator > 0.25f) accumulator = 0.25f;
    while (accumulator >= fixedDt) {
      m_game.update(fixedDt);
      accumulator -= fixedDt;
    }

    m_game.draw();
    m_window.display();
  }
}
