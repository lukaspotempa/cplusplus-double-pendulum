#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

const sf::Vector2u windowSize(1580, 760);
const float g = 600.f;
float damping = 0.9f;
float sensitivity = 500.00f;


class Cart : public sf::Drawable, public sf::Transformable {
public:
    Cart() {
        cartSize = { 100.f, 50.f };
        cartRect.setSize(cartSize);
        cartRect.setFillColor(sf::Color::White);

        sf::Vector2f newPos = {
            ((float)windowSize.x - cartSize.x) / 2,
            ((float)windowSize.y - cartSize.y) / 2
        };
        setPosition(newPos);
    }

    void update(float windowWidth, float dt, float xDDot) {
        velocity += xDDot * dt;
        velocity *= damping;

        sf::Vector2f pos = getPosition();
        pos.x = std::clamp(pos.x + velocity * dt, 0.f, windowWidth - cartSize.x);
        setPosition(pos);
    }

    float getMass() const { return mass; }

    sf::Vector2f getPivot() const {
        sf::Vector2f pos = getPosition();
        return { pos.x + cartSize.x / 2.f, pos.y };
    }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(cartRect, states);
    }

private:
    sf::RectangleShape cartRect;
    sf::Vector2f cartSize;
    float velocity = 0.f;
    float mass = 5.f;
};

class Pendulum : public sf::Drawable, public sf::Transformable {
public:
    Pendulum() {
        bob.setRadius(bobRadius);
        bob.setOrigin({ bobRadius, bobRadius });
        bob.setFillColor(sf::Color::Red);
    }

    void update(float dt, float xDDot, sf::Vector2f pivot) {
        float pDamp = .999f;
        const float scaleCoef = 0.1f;
        float thetaDDot = (g * std::sin(theta) - xDDot * scaleCoef * std::cos(theta)) / length;
        thetaDot += thetaDDot * dt;
        thetaDot *= pDamp;
        theta += thetaDot * dt;

        pivotPos = pivot;
    }

    float getTheta() const { return theta; }
    float getThetaDot() const { return thetaDot; }
    float getMass() const { return mass; }
    float getLength() const { return length; }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        sf::Vector2f bobPos = {
            pivotPos.x + length * std::sin(theta),
            pivotPos.y - length * std::cos(theta)
        };

        sf::Vertex line[2] = {
            sf::Vertex{ pivotPos, sf::Color::White },
            sf::Vertex{ bobPos,   sf::Color::White }
        };
        target.draw(line, 2, sf::PrimitiveType::Lines);

        bob.setPosition(bobPos);
        target.draw(bob, states);
    }

private:
    float theta = 3.14f;   
    float thetaDot = 0.f;
    float length = 200.f;
    float bobRadius = 15.f;
    float mass = 1.f;

    sf::Vector2f pivotPos;
    mutable sf::CircleShape bob;
};

int main() {
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    sf::RenderWindow window(
        sf::VideoMode(windowSize),
        "Inverted Pendulum",
        sf::State::Windowed,
        settings
    );
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    sf::Vector2i windowCenter(windowSize.x / 2, windowSize.y / 2);
    sf::Mouse::setPosition(windowCenter, window);

    Cart cart;
    Pendulum pendulum;
    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f); 

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        float delta = static_cast<float>(mousePos.x - windowCenter.x);
        sf::Mouse::setPosition(windowCenter, window);

        float F = delta * sensitivity;
        float theta = pendulum.getTheta();
        float thetaDot = pendulum.getThetaDot();
        float m = pendulum.getMass();
        float M = cart.getMass();
        float L = pendulum.getLength();

   
        float sinT = std::sin(theta);
        float cosT = std::cos(theta);
        float denom = M + m - m * cosT * cosT;

        float xDDot = F / M;


        cart.update(windowSize.x, dt, xDDot);
        pendulum.update(dt, xDDot, cart.getPivot());

        window.clear(sf::Color::Black);
        window.draw(cart);
        window.draw(pendulum);
        window.display();
    }

    return 0;
}