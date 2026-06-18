#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <random>
#include <string>
#include <vector>

namespace util {

constexpr float PI = 3.14159265358979323846f;

float length(sf::Vector2f v);
float lengthSquared(sf::Vector2f v);
sf::Vector2f normalize(sf::Vector2f v);
float dot(sf::Vector2f a, sf::Vector2f b);
float cross(sf::Vector2f a, sf::Vector2f b);
sf::Vector2f rotate(sf::Vector2f v, float radians);
float distance(sf::Vector2f a, sf::Vector2f b);
float clamp(float value, float lo, float hi);
int randomInt(int lo, int hi);
float randomFloat(float lo, float hi);
bool loadDefaultFont(sf::Font& font);
sf::String utf8(const std::string& text);

}  // namespace util
