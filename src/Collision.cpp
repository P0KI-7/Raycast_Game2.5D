#include "Collision.hpp"
#include "Map.hpp"
#include "Utilities.hpp"
#include <cmath>

namespace collision {

bool circleCircle(sf::Vector2f a, float ar, sf::Vector2f b, float br) {
    const sf::Vector2f d{a.x - b.x, a.y - b.y};
    const float r = ar + br;
    return d.x * d.x + d.y * d.y <= r * r;
}

bool lineCircle(sf::Vector2f origin, sf::Vector2f dirNormalized, float maxDistance, sf::Vector2f center, float radius, float* alongDistance) {
    const sf::Vector2f toCenter{center.x - origin.x, center.y - origin.y};
    const float t = util::dot(toCenter, dirNormalized);
    if (t < 0.f || t > maxDistance) return false;
    const sf::Vector2f closest{origin.x + dirNormalized.x * t, origin.y + dirNormalized.y * t};
    const float distSq = util::lengthSquared({center.x - closest.x, center.y - closest.y});
    if (distSq <= radius * radius) {
        if (alongDistance) *alongDistance = t;
        return true;
    }
    return false;
}

sf::Vector2f slideMove(sf::Vector2f pos, sf::Vector2f delta, float radius, const Map& map) {
    sf::Vector2f candidate{pos.x + delta.x, pos.y};
    if (!map.isBlockedCircle(candidate, radius)) pos.x = candidate.x;
    candidate = {pos.x, pos.y + delta.y};
    if (!map.isBlockedCircle(candidate, radius)) pos.y = candidate.y;
    return pos;
}

} // namespace collision
