#include "SinglePendulum.hpp"
#include <cmath>

SinglePendulum::SinglePendulum() : trail(sf::Color(255, 200, 100)) {
    bob.setRadius(bobRadius);
    bob.setOrigin({ bobRadius, bobRadius });
    bob.setFillColor(Constants::COLOR_BOB);
    bob.setOutlineThickness(2.f);
    bob.setOutlineColor(Constants::COLOR_BOB_OUTLINE);

    hinge.setRadius(hingeRadius);
    hinge.setOrigin({ hingeRadius, hingeRadius });
    hinge.setFillColor(Constants::COLOR_HINGE);
    hinge.setOutlineThickness(1.f);
    hinge.setOutlineColor(Constants::COLOR_HINGE_OUTLINE);
}

void SinglePendulum::computeAcceleration(float t, float w, float aCart, float& alpha) {
    // alpha = -(g/L)*sin(theta) + (aCart/L)*cos(theta)
    float s = std::sin(t);
    float c = std::cos(t);
    
    alpha = -(Constants::GRAVITY / length) * s + (aCart / length) * c;
}

void SinglePendulum::update(float dt, float xDDot, sf::Vector2f pivot) {
    pivotPos = pivot;
    
    float aCart = xDDot * 0.15f;
    
    // RK4 integration
    float k1_t, k1_w;
    float k2_t, k2_w;
    float k3_t, k3_w;
    float k4_t, k4_w;
    float a;
    
    // k1
    k1_t = thetaDot;
    computeAcceleration(theta, thetaDot, aCart, a);
    k1_w = a;
    
    // k2
    k2_t = thetaDot + 0.5f * dt * k1_w;
    computeAcceleration(theta + 0.5f * dt * k1_t, k2_t, aCart, a);
    k2_w = a;
    
    // k3
    k3_t = thetaDot + 0.5f * dt * k2_w;
    computeAcceleration(theta + 0.5f * dt * k2_t, k3_t, aCart, a);
    k3_w = a;
    
    // k4
    k4_t = thetaDot + dt * k3_w;
    computeAcceleration(theta + dt * k3_t, k4_t, aCart, a);
    k4_w = a;
    
    // Update state
    theta += (dt / 6.0f) * (k1_t + 2.0f * k2_t + 2.0f * k3_t + k4_t);
    thetaDot += (dt / 6.0f) * (k1_w + 2.0f * k2_w + 2.0f * k3_w + k4_w);
    
    // Apply damping
    thetaDot *= Constants::PENDULUM_DAMPING;
    
    // Update trail
    trail.addPoint(getBobPos());
    trail.update(dt);
}

void SinglePendulum::reset() {
    theta = initialTheta;
    thetaDot = initialThetaDot;
    trail.clear();
}

sf::Vector2f SinglePendulum::getBobPos() const {
    return {
        pivotPos.x + length * std::sin(theta),
        pivotPos.y + length * std::cos(theta)
    };
}

void SinglePendulum::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Draw trail first (behind everything)
    target.draw(trail, states);
    
    sf::Vector2f bobPos = getBobPos();

    auto drawLine = [&](sf::Vector2f start, sf::Vector2f end, float width, sf::Color fill, sf::Color outline) {
        sf::Vector2f diff = { end.x - start.x, end.y - start.y };
        float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        lineShape.setSize({ len, width });
        lineShape.setOrigin({ 0.f, width / 2.f });
        lineShape.setPosition(start);
        lineShape.setRotation(sf::radians(std::atan2(diff.y, diff.x)));

        lineShape.setFillColor(fill);
        lineShape.setOutlineThickness(1.5f);
        lineShape.setOutlineColor(outline);

        target.draw(lineShape, states);
    };

    // Draw rod
    drawLine(pivotPos, bobPos, 8.f, Constants::COLOR_ROD, Constants::COLOR_ROD_OUTLINE);

    // Draw bob
    bob.setPosition(bobPos);
    target.draw(bob, states);

    // Draw hinge
    hinge.setPosition(pivotPos);
    target.draw(hinge, states);
}
