#include "Utilities.hpp"

#include <algorithm>
#include <filesystem>

namespace util {

float length(sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); }
float lengthSquared(sf::Vector2f v) { return v.x * v.x + v.y * v.y; }

sf::Vector2f normalize(sf::Vector2f v) {
  const float len = length(v);
  if (len <= 0.00001f) return {0.f, 0.f};
  return {v.x / len, v.y / len};
}

float dot(sf::Vector2f a, sf::Vector2f b) { return a.x * b.x + a.y * b.y; }
float cross(sf::Vector2f a, sf::Vector2f b) { return a.x * b.y - a.y * b.x; }

sf::Vector2f rotate(sf::Vector2f v, float radians) {
  const float c = std::cos(radians);
  const float s = std::sin(radians);
  return {v.x * c - v.y * s, v.x * s + v.y * c};
}

float distance(sf::Vector2f a, sf::Vector2f b) {
  return length({a.x - b.x, a.y - b.y});
}
float clamp(float value, float lo, float hi) {
  return std::max(lo, std::min(value, hi));
}

int randomInt(int lo, int hi) {
  static std::mt19937 rng{std::random_device{}()};
  std::uniform_int_distribution<int> dist(lo, hi);
  return dist(rng);
}

float randomFloat(float lo, float hi) {
  static std::mt19937 rng{std::random_device{}()};
  std::uniform_real_distribution<float> dist(lo, hi);
  return dist(rng);
}

bool loadDefaultFont(sf::Font& font) {
  const std::vector<std::string> candidates = {
      "assets/fonts/Arial.ttf",
      "assets/fonts/DejaVuSans.ttf",
      "C:/Windows/Fonts/arial.ttf",
      "C:/Windows/Fonts/segoeui.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
      "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
      "/System/Library/Fonts/Supplemental/Arial.ttf"};
  for (const auto& path : candidates) {
    if (std::filesystem::exists(path) && font.loadFromFile(path)) return true;
  }
  return false;
}

sf::String utf8(const std::string& text) {
  return sf::String::fromUtf8(text.begin(), text.end());
}

}  // namespace util
