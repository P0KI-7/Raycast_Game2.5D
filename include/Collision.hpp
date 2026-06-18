#pragma once

#include <SFML/System/Vector2.hpp>

class Map;

namespace collision {

bool circleCircle(sf::Vector2f a, float ar, sf::Vector2f b, float br);
bool lineCircle(sf::Vector2f origin, sf::Vector2f dirNormalized, float maxDistance, sf::Vector2f center, float radius, float* alongDistance = nullptr);
sf::Vector2f slideMove(sf::Vector2f pos, sf::Vector2f delta, float radius, const Map& map);

} // namespace collision
