#include "Cart.hpp"
#include <algorithm>

Cart::Cart() {
    cartSize = { 120.f, 30.f };
    cartRect.setSize(cartSize);
    cartRect.setFillColor(Constants::COLOR_CART);
    cartRect.setOutlineColor(Constants::COLOR_CART_OUTLINE);
    cartRect.setOutlineThickness(3.f);

    sf::Vector2f cartPos = {
        ((float)Constants::WINDOW_WIDTH - cartSize.x) / 2,
        ((float)Constants::WINDOW_HEIGHT - cartSize.y) / 2
    };
    setPosition(cartPos);
}

float Cart::update(float dt, float xDDot) {
    if (dt <= 0.f) return xDDot;

    sf::Vector2f pos = getPosition();
    float leftBound = Constants::CENTER_LINE_WIDTH / 2.f - 5.f;
    float rightBound = Constants::WINDOW_WIDTH - cartSize.x - Constants::CENTER_LINE_WIDTH / 2.f;

    if (pos.x <= leftBound && xDDot < 0.f) {
        xDDot = 0.f;
    } else if (pos.x >= rightBound && xDDot > 0.f) {
        xDDot = 0.f;
    }

    velocity += xDDot * dt;
    velocity *= damping;

    float newX = pos.x + velocity * dt;
    float clampedX = std::clamp(newX, leftBound, rightBound);

    if (newX != clampedX) {
        velocity = 0.f;
    }

    setPosition({ clampedX, pos.y });

    return xDDot;
}

void Cart::reset() {
    velocity = 0.f;
    sf::Vector2f cartPos = {
        ((float)Constants::WINDOW_WIDTH - cartSize.x) / 2,
        ((float)Constants::WINDOW_HEIGHT - cartSize.y) / 2
    };
    setPosition(cartPos);
}

sf::Vector2f Cart::getPivot() const {
    sf::Vector2f pos = getPosition();
    return { pos.x + cartSize.x / 2.f, pos.y + cartSize.y / 2.f };
}

void Cart::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(cartRect, states);
}
