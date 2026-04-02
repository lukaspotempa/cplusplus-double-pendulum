#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include "Constants.hpp"

struct TrailPoint {
    sf::Vector2f position;
    float alpha;
};

class Trail : public sf::Drawable {
public:
    Trail(sf::Color baseColor = sf::Color::White);
    
    void addPoint(sf::Vector2f position);
    void update(float dt);
    void clear();
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }
    void setColor(sf::Color color) { m_baseColor = color; }
    
protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
private:
    std::deque<TrailPoint> m_points;
    sf::Color m_baseColor;
    bool m_enabled = false;
    size_t m_maxPoints = Constants::MAX_TRAIL_POINTS;
    float m_fadeRate = Constants::TRAIL_FADE_RATE;
};
