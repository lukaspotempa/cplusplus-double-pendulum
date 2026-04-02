#include "DoublePendulum.hpp"
#include <cmath>

DoublePendulum::DoublePendulum() : trail(sf::Color(100, 200, 255)) {
    bob1.setRadius(bobRadius1);
    bob1.setOrigin({ bobRadius1, bobRadius1 });
    bob1.setFillColor(Constants::COLOR_BOB);
    bob1.setOutlineThickness(2.f);
    bob1.setOutlineColor(Constants::COLOR_BOB_OUTLINE);

    bob2.setRadius(bobRadius2);
    bob2.setOrigin({ bobRadius2, bobRadius2 });
    bob2.setFillColor(Constants::COLOR_BOB);
    bob2.setOutlineThickness(2.f);
    bob2.setOutlineColor(Constants::COLOR_BOB_OUTLINE);

    hinge.setRadius(hingeRadius);
    hinge.setOrigin({ hingeRadius, hingeRadius });
    hinge.setFillColor(Constants::COLOR_HINGE);
    hinge.setOutlineThickness(1.f);
    hinge.setOutlineColor(Constants::COLOR_HINGE_OUTLINE);
}

void DoublePendulum::computeAccelerations(float t1, float t2, float w1, float w2, float aCart,
                                          float& alpha1, float& alpha2) {
    float dTheta = t1 - t2;
    float s1 = std::sin(t1);
    float s2 = std::sin(t2);
    float sd = std::sin(dTheta);
    float cd = std::cos(dTheta);
    
    float M = m1 + m2;
    float g = Constants::GRAVITY;
    
    float denom = L1 * (M - m2 * cd * cd);

    alpha1 = (-m2 * cd * (L1 * w1 * w1 * sd - g * s2)
              - m2 * L2 * w2 * w2 * sd
              - M * g * s1
              + M * aCart * std::cos(t1)) / denom;
    
    float denom2 = L2 * (M - m2 * cd * cd);
    alpha2 = (m2 * cd * (L2 * w2 * w2 * sd + g * s1)
              + M * L1 * w1 * w1 * sd
              - M * g * s2
              + m2 * aCart * std::cos(t2) * cd
              - M * aCart * std::cos(t1) * cd
              + M * aCart * std::cos(t2)) / denom2;
}

void DoublePendulum::update(float dt, float xDDot, sf::Vector2f pivot) {
    pivotPos = pivot;
    
    float aCart = xDDot * 0.15f;
    
    float k1_t1, k1_t2, k1_w1, k1_w2;
    float k2_t1, k2_t2, k2_w1, k2_w2;
    float k3_t1, k3_t2, k3_w1, k3_w2;
    float k4_t1, k4_t2, k4_w1, k4_w2;
    float a1, a2;
    
    // k1
    k1_t1 = theta1Dot;
    k1_t2 = theta2Dot;
    computeAccelerations(theta1, theta2, theta1Dot, theta2Dot, aCart, a1, a2);
    k1_w1 = a1;
    k1_w2 = a2;
    
    // k2
    k2_t1 = theta1Dot + 0.5f * dt * k1_w1;
    k2_t2 = theta2Dot + 0.5f * dt * k1_w2;
    computeAccelerations(theta1 + 0.5f * dt * k1_t1, theta2 + 0.5f * dt * k1_t2,
                        k2_t1, k2_t2, aCart, a1, a2);
    k2_w1 = a1;
    k2_w2 = a2;
    
    // k3
    k3_t1 = theta1Dot + 0.5f * dt * k2_w1;
    k3_t2 = theta2Dot + 0.5f * dt * k2_w2;
    computeAccelerations(theta1 + 0.5f * dt * k2_t1, theta2 + 0.5f * dt * k2_t2,
                        k3_t1, k3_t2, aCart, a1, a2);
    k3_w1 = a1;
    k3_w2 = a2;
    
    // k4
    k4_t1 = theta1Dot + dt * k3_w1;
    k4_t2 = theta2Dot + dt * k3_w2;
    computeAccelerations(theta1 + dt * k3_t1, theta2 + dt * k3_t2,
                        k4_t1, k4_t2, aCart, a1, a2);
    k4_w1 = a1;
    k4_w2 = a2;
    
    // Update state
    theta1 += (dt / 6.0f) * (k1_t1 + 2.0f * k2_t1 + 2.0f * k3_t1 + k4_t1);
    theta2 += (dt / 6.0f) * (k1_t2 + 2.0f * k2_t2 + 2.0f * k3_t2 + k4_t2);
    theta1Dot += (dt / 6.0f) * (k1_w1 + 2.0f * k2_w1 + 2.0f * k3_w1 + k4_w1);
    theta2Dot += (dt / 6.0f) * (k1_w2 + 2.0f * k2_w2 + 2.0f * k3_w2 + k4_w2);
    
    // Apply damping
    theta1Dot *= Constants::PENDULUM_DAMPING;
    theta2Dot *= Constants::PENDULUM_DAMPING;
    
    // Update trail
    trail.addPoint(getBob2Pos());
    trail.update(dt);
}

void DoublePendulum::reset() {
    theta1 = initialTheta1;
    theta2 = initialTheta2;
    theta1Dot = initialTheta1Dot;
    theta2Dot = initialTheta2Dot;
    trail.clear();
}

sf::Vector2f DoublePendulum::getBob1Pos() const {
    return {
        pivotPos.x + L1 * std::sin(theta1),
        pivotPos.y + L1 * std::cos(theta1)
    };
}

sf::Vector2f DoublePendulum::getBob2Pos() const {
    sf::Vector2f bob1Pos = getBob1Pos();
    return {
        bob1Pos.x + L2 * std::sin(theta2),
        bob1Pos.y + L2 * std::cos(theta2)
    };
}

void DoublePendulum::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Draw trail
    target.draw(trail, states);
    
    sf::Vector2f bob1Pos = getBob1Pos();
    sf::Vector2f bob2Pos = getBob2Pos();

    auto drawLine = [&](sf::Vector2f start, sf::Vector2f end, float width, sf::Color fill, sf::Color outline) {
        sf::Vector2f diff = { end.x - start.x, end.y - start.y };
        float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        lineShape.setSize({ length, width });
        lineShape.setOrigin({ 0.f, width / 2.f });
        lineShape.setPosition(start);
        lineShape.setRotation(sf::radians(std::atan2(diff.y, diff.x)));

        lineShape.setFillColor(fill);
        lineShape.setOutlineThickness(1.5f);
        lineShape.setOutlineColor(outline);

        target.draw(lineShape, states);
    };

    // Draw rods
    drawLine(pivotPos, bob1Pos, 8.f, Constants::COLOR_ROD, Constants::COLOR_ROD_OUTLINE);
    drawLine(bob1Pos, bob2Pos, 6.f, Constants::COLOR_ROD, Constants::COLOR_ROD_OUTLINE);

    // Draw bobs
    bob1.setPosition(bob1Pos);
    target.draw(bob1, states);

    bob2.setPosition(bob2Pos);
    target.draw(bob2, states);

    // Draw hinge
    hinge.setPosition(pivotPos);
    target.draw(hinge, states);
}
