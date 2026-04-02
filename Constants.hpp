#pragma once

#include <SFML/Graphics.hpp>

namespace Constants {
    // Window settings
    constexpr unsigned int WINDOW_WIDTH = 1580;
    constexpr unsigned int WINDOW_HEIGHT = 760;
    const sf::Vector2u WINDOW_SIZE(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Physics
    constexpr float GRAVITY = 500.f;
    constexpr float DEFAULT_DAMPING = 0.95f;
    constexpr float PENDULUM_DAMPING = 0.998f;
    constexpr float SENSITIVITY = 800.0f;
    constexpr float CENTER_LINE_WIDTH = 200.f;
    
    // Colors
    const sf::Color COLOR_BACKGROUND(30, 39, 46);
    const sf::Color COLOR_GREY(65, 65, 65);
    const sf::Color COLOR_TRACK(149, 165, 166);
    const sf::Color COLOR_TRACK_OUTLINE(127, 140, 141);
    const sf::Color COLOR_CART(41, 128, 185);
    const sf::Color COLOR_CART_OUTLINE(31, 97, 141);
    const sf::Color COLOR_BOB(255, 255, 255);
    const sf::Color COLOR_BOB_OUTLINE(185, 185, 185);
    const sf::Color COLOR_ROD(189, 195, 199);
    const sf::Color COLOR_ROD_OUTLINE(127, 140, 141);
    const sf::Color COLOR_HINGE(149, 165, 166);
    const sf::Color COLOR_HINGE_OUTLINE(127, 140, 141);
    
    // Trail settings
    constexpr size_t MAX_TRAIL_POINTS = 500;
    constexpr float TRAIL_FADE_RATE = 0.5f;
}
