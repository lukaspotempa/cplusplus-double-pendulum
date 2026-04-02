#include "Application.hpp"
#include <algorithm>

Application::Application() 
    : m_window(sf::VideoMode(Constants::WINDOW_SIZE), "Pendulum Simulator", sf::State::Windowed,
               sf::ContextSettings(0, 0, 8))
{
    m_window.setFramerateLimit(60);
    m_window.setMouseCursorVisible(false);
    
    m_windowCenter = sf::Vector2i(Constants::WINDOW_WIDTH / 2, Constants::WINDOW_HEIGHT / 2);
    sf::Mouse::setPosition(m_windowCenter, m_window);
    
    // Setup center line (track)
    m_centerLine.setSize({ Constants::WINDOW_WIDTH - Constants::CENTER_LINE_WIDTH, 8.f });
    m_centerLine.setFillColor(Constants::COLOR_TRACK);
    m_centerLine.setOutlineThickness(2.f);
    m_centerLine.setOutlineColor(Constants::COLOR_TRACK_OUTLINE);
    sf::FloatRect bounds = m_centerLine.getLocalBounds();
    m_centerLine.setOrigin({ bounds.size.x / 2, bounds.size.y / 2 });
    m_centerLine.setPosition({ static_cast<float>(m_windowCenter.x), static_cast<float>(m_windowCenter.y) });
    
    // Try to load font
    if (m_font.openFromFile("C:/Windows/Fonts/segoeui.ttf")) {
        m_fontLoaded = true;
    } else if (m_font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        m_fontLoaded = true;
    }
    
    // Initialize pendulum (default to double)
    m_pendulum = std::make_unique<DoublePendulum>();
    m_currentMode = PendulumMode::Double;
    
    // Setup menu
    if (m_fontLoaded) {
        setupMenu();
        
        // Setup help text
        m_helpText = std::make_unique<sf::Text>(m_font, "Press ESC for menu", 18);
        m_helpText->setFillColor(sf::Color(150, 150, 150));
        m_helpText->setPosition({ 10.f, 10.f });
        
        // Setup mode text
        m_modeText = std::make_unique<sf::Text>(m_font, "Mode: Double Pendulum", 18);
        m_modeText->setFillColor(sf::Color(150, 150, 150));
        m_modeText->setPosition({ 10.f, Constants::WINDOW_HEIGHT - 30.f });
    }
}

void Application::setupMenu() {
    m_menu = std::make_unique<Menu>(m_font);
    
    m_menu->addItem("Single Pendulum", [this]() {
        switchPendulumMode(PendulumMode::Single);
        setMenuVisible(false);
    });
    
    m_menu->addItem("Double Pendulum", [this]() {
        switchPendulumMode(PendulumMode::Double);
        setMenuVisible(false);
    });
    
    m_menu->addToggleItem("Enable Trail", false, [this](bool enabled) {
        toggleTrail(enabled);
    });
    
    m_menu->addItem("Reset Simulation", [this]() {
        resetSimulation();
        setMenuVisible(false);
    });
    
    m_menu->addItem("Close Menu", [this]() {
        setMenuVisible(false);
    });
}

void Application::setMenuVisible(bool visible) {
    if (m_menu) {
        if (visible) {
            m_menu->show();
        } else {
            m_menu->hide();
        }
        
        // Update mouse cursor visibility
        m_window.setMouseCursorVisible(visible);
        
        // Re-center and lock mouse when hiding menu
        if (!visible) {
            sf::Mouse::setPosition(m_windowCenter, m_window);
        }
    }
}

void Application::switchPendulumMode(PendulumMode mode) {
    if (mode == m_currentMode) return;
    
    m_currentMode = mode;
    
    if (mode == PendulumMode::Single) {
        m_pendulum = std::make_unique<SinglePendulum>();
        if (m_modeText) m_modeText->setString("Mode: Single Pendulum");
    } else {
        m_pendulum = std::make_unique<DoublePendulum>();
        if (m_modeText) m_modeText->setString("Mode: Double Pendulum");
    }
    
    // Apply trail state to new pendulum
    m_pendulum->setTrailEnabled(m_trailEnabled);
    
    // Reset cart position
    m_cart.reset();
}

void Application::toggleTrail(bool enabled) {
    m_trailEnabled = enabled;
    if (m_pendulum) {
        m_pendulum->setTrailEnabled(enabled);
    }
}

void Application::resetSimulation() {
    m_cart.reset();
    if (m_pendulum) {
        m_pendulum->reset();
    }
}

void Application::run() {
    while (m_window.isOpen()) {
        processEvents();
        
        float dt = m_clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);
        
        update(dt);
        render();
    }
}

void Application::processEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                if (m_menu) {
                    setMenuVisible(!m_menu->isVisible());
                }
            }
            else if (m_menu && m_menu->isVisible()) {
                m_menu->handleKeyPressed(keyPressed->code);
            }
            else if (keyPressed->code == sf::Keyboard::Key::R) {
                resetSimulation();
            }
            else if (keyPressed->code == sf::Keyboard::Key::T) {
                m_trailEnabled = !m_trailEnabled;
                toggleTrail(m_trailEnabled);
                if (m_menu) {
                    m_menu->setToggleState(2, m_trailEnabled); // Index 2 is the trail toggle
                }
            }
            else if (keyPressed->code == sf::Keyboard::Key::Num1) {
                switchPendulumMode(PendulumMode::Single);
            }
            else if (keyPressed->code == sf::Keyboard::Key::Num2) {
                switchPendulumMode(PendulumMode::Double);
            }
        }
    }
}

void Application::update(float dt) {
    if (m_menu && m_menu->isVisible()) {
        m_menu->update();
        return; // Pause simulation while menu is open
    }
    
    sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
    float delta = static_cast<float>(mousePos.x - m_windowCenter.x);
    sf::Mouse::setPosition(m_windowCenter, m_window);
    
    float F = delta * Constants::SENSITIVITY;
    float M = m_cart.getMass();
    float xDDot = F / M;
    
    float effectiveXDDot = m_cart.update(dt, xDDot);
    
    if (m_pendulum) {
        m_pendulum->update(dt, effectiveXDDot, m_cart.getPivot());
    }
}

void Application::render() {
    m_window.clear(Constants::COLOR_BACKGROUND);
    
    m_window.draw(m_centerLine);
    m_window.draw(m_cart);
    
    if (m_pendulum) {
        m_window.draw(*m_pendulum);
    }
    
    // Draw UI
    if (m_fontLoaded) {
        if (m_helpText) m_window.draw(*m_helpText);
        if (m_modeText) m_window.draw(*m_modeText);
        
        if (m_menu) {
            m_window.draw(*m_menu);
        }
    }
    
    m_window.display();
}
