#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"

class Cart : public sf::Drawable, public sf::Transformable {
public:
    Cart();
    
    float update(float dt, float xDDot);
    void reset();
    
    float getMass() const { return mass; }
    float getVelocity() const { return velocity; }
    sf::Vector2f getPivot() const;
    
protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
private:
    sf::RectangleShape cartRect;
    sf::Vector2f cartSize;
    float velocity = 0.f;
    float mass = 5.f;
    float damping = Constants::DEFAULT_DAMPING;
};
