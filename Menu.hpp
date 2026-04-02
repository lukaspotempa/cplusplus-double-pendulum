#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <string>

enum class PendulumMode {
    Single,
    Double
};

struct MenuItem {
    std::string label;
    bool isToggle = false;
    bool toggleState = false;
    std::function<void()> action;
    std::function<void(bool)> toggleAction;
};

class Menu : public sf::Drawable {
public:
    Menu(const sf::Font& font);
    
    void addItem(const std::string& label, std::function<void()> action);
    void addToggleItem(const std::string& label, bool initialState, std::function<void(bool)> action);
    
    void handleKeyPressed(sf::Keyboard::Key key);
    void update();
    
    void show() { m_visible = true; }
    void hide() { m_visible = false; }
    void toggle() { m_visible = !m_visible; }
    bool isVisible() const { return m_visible; }
    
    void setToggleState(size_t index, bool state);
    
protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
private:
    void updateLayout();
    
    std::vector<MenuItem> m_items;
    const sf::Font& m_font;
    mutable std::vector<sf::Text> m_texts;
    mutable sf::RectangleShape m_background;
    mutable sf::RectangleShape m_itemHighlight;
    
    bool m_visible = false;
    int m_selectedIndex = 0;
    
    // Layout
    float m_padding = 20.f;
    float m_itemHeight = 40.f;
    float m_menuWidth = 350.f;
};
