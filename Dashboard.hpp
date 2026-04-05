#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include "Graph.hpp"
#include "TrainingHistory.hpp"

// UI Button component
class Button : public sf::Drawable {
public:
    Button(float x, float y, float width, float height, const std::string& label)
        : m_bounds({x, y}, {width, height})
        , m_label(label)
    {
        m_shape.setPosition({x, y});
        m_shape.setSize({width, height});
        m_shape.setFillColor(Constants::COLOR_ZINC_700);
        m_shape.setOutlineColor(Constants::COLOR_ZINC_600);
        m_shape.setOutlineThickness(1.0f);
    }
    
    void setFont(const sf::Font& font) {
        m_font = &font;
    }
    
    void setColors(sf::Color normal, sf::Color hover, sf::Color active) {
        m_normalColor = normal;
        m_hoverColor = hover;
        m_activeColor = active;
        m_shape.setFillColor(normal);
    }
    
    bool contains(float x, float y) const {
        return m_bounds.contains({x, y});
    }
    
    void setHovered(bool hovered) {
        m_hovered = hovered;
        updateColor();
    }
    
    void setActive(bool active) {
        m_active = active;
        updateColor();
    }
    
    void setEnabled(bool enabled) {
        m_enabled = enabled;
        updateColor();
    }
    
    bool isEnabled() const { return m_enabled; }
    
    void setOnClick(std::function<void()> callback) {
        m_onClick = callback;
    }
    
    void click() {
        if (m_enabled && m_onClick) {
            m_onClick();
        }
    }

private:
    void updateColor() {
        if (!m_enabled) {
            m_shape.setFillColor(Constants::COLOR_ZINC_800);
        } else if (m_active) {
            m_shape.setFillColor(m_activeColor);
        } else if (m_hovered) {
            m_shape.setFillColor(m_hoverColor);
        } else {
            m_shape.setFillColor(m_normalColor);
        }
    }
    
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(m_shape, states);
        
        if (m_font) {
            sf::Text text(*m_font, m_label, 13);
            text.setFillColor(m_enabled ? Constants::COLOR_ZINC_300 : Constants::COLOR_ZINC_500);
            
            float textX = m_bounds.position.x + (m_bounds.size.x - text.getLocalBounds().size.x) / 2.0f;
            float textY = m_bounds.position.y + (m_bounds.size.y - text.getLocalBounds().size.y) / 2.0f - 2.0f;
            text.setPosition({textX, textY});
            target.draw(text, states);
        }
    }
    
    sf::FloatRect m_bounds;
    sf::RectangleShape m_shape;
    std::string m_label;
    const sf::Font* m_font = nullptr;
    
    sf::Color m_normalColor = Constants::COLOR_ZINC_700;
    sf::Color m_hoverColor = Constants::COLOR_ZINC_600;
    sf::Color m_activeColor = Constants::COLOR_EMERALD_600;
    
    bool m_hovered = false;
    bool m_active = false;
    bool m_enabled = true;
    
    std::function<void()> m_onClick;
};

// Numeric input field
class NumberInput : public sf::Drawable {
public:
    NumberInput(float x, float y, float width, float height, int initialValue, int minVal, int maxVal)
        : m_bounds({x, y}, {width, height})
        , m_value(initialValue)
        , m_minValue(minVal)
        , m_maxValue(maxVal)
        , m_leftButton(x, y, 25, height, "<")
        , m_rightButton(x + width - 25, y, 25, height, ">")
    {
        if (m_minValue > m_maxValue) {
            std::swap(m_minValue, m_maxValue);
        }

        m_value = std::clamp(m_value, m_minValue, m_maxValue);
        
        m_background.setPosition({x + 25, y});
        m_background.setSize({width - 50, height});
        m_background.setFillColor(Constants::COLOR_ZINC_800);
        m_background.setOutlineColor(Constants::COLOR_ZINC_600);
        m_background.setOutlineThickness(1.0f);
        
        m_leftButton.setOnClick([this]() { decrement(); });
        m_rightButton.setOnClick([this]() { increment(); });
    }
    
    void setFont(const sf::Font& font) {
        m_font = &font;
        m_leftButton.setFont(font);
        m_rightButton.setFont(font);
    }
    
    int getValue() const { return m_value; }
    int getMinValue() const { return m_minValue; }
    int getMaxValue() const { return m_maxValue; }
    
    void setValue(int value) {
        if (m_minValue > m_maxValue) {
            std::swap(m_minValue, m_maxValue);
        }
        m_value = std::clamp(value, m_minValue, m_maxValue);
    }
    
    void increment(int amount = 1) {
        setValue(m_value + amount);
    }
    
    void decrement(int amount = 1) {
        setValue(m_value - amount);
    }
    
    void setRange(int minVal, int maxVal) {
        if (minVal > maxVal) {
            std::swap(minVal, maxVal);
        }
        m_minValue = minVal;
        m_maxValue = maxVal;
        setValue(m_value);
    }
    
    bool handleClick(float x, float y) {
        if (m_leftButton.contains(x, y)) {
            m_leftButton.click();
            return true;
        }
        if (m_rightButton.contains(x, y)) {
            m_rightButton.click();
            return true;
        }

        if (m_bounds.contains({x, y})) {
            m_editing = true;
            m_inputBuffer = std::to_string(m_value);
            return true;
        }
        return false;
    }
    
    void handleMouseMove(float x, float y) {
        m_leftButton.setHovered(m_leftButton.contains(x, y));
        m_rightButton.setHovered(m_rightButton.contains(x, y));
    }
    
    bool handleTextInput(char c) {
        if (!m_editing) return false;
        
        if (c >= '0' && c <= '9') {
            if (m_inputBuffer.length() < 4) {
                m_inputBuffer += c;
            }
            return true;
        }
        return false;
    }
    
    bool handleKey(sf::Keyboard::Key key) {
        if (!m_editing) return false;
        
        if (key == sf::Keyboard::Key::Enter) {
            if (!m_inputBuffer.empty()) {
                setValue(std::stoi(m_inputBuffer));
            }
            m_editing = false;
            return true;
        }
        if (key == sf::Keyboard::Key::Escape) {
            m_editing = false;
            return true;
        }
        if (key == sf::Keyboard::Key::Backspace) {
            if (!m_inputBuffer.empty()) {
                m_inputBuffer.pop_back();
            }
            return true;
        }
        return false;
    }
    
    void stopEditing() {
        if (m_editing && !m_inputBuffer.empty()) {
            setValue(std::stoi(m_inputBuffer));
        }
        m_editing = false;
    }
    
    bool isEditing() const { return m_editing; }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(m_background, states);
        target.draw(m_leftButton, states);
        target.draw(m_rightButton, states);
        
        if (m_font) {
            std::string displayText = m_editing ? m_inputBuffer + "_" : std::to_string(m_value);
            sf::Text text(*m_font, displayText, 14);
            text.setFillColor(Constants::COLOR_ZINC_300);
            
            float textX = m_bounds.position.x + 25 + (m_bounds.size.x - 50 - text.getLocalBounds().size.x) / 2.0f;
            float textY = m_bounds.position.y + (m_bounds.size.y - text.getLocalBounds().size.y) / 2.0f - 2.0f;
            text.setPosition({textX, textY});
            target.draw(text, states);
        }
    }
    
    sf::FloatRect m_bounds;
    sf::RectangleShape m_background;
    int m_value;
    int m_minValue;
    int m_maxValue;
    
    Button m_leftButton;
    Button m_rightButton;
    
    const sf::Font* m_font = nullptr;
    bool m_editing = false;
    std::string m_inputBuffer;
};


struct AgentListItem {
    int originalIndex = 0;
    int rank = 0;
    float fitness = 0.0f;
    bool isSelected = false;
    bool isBest = false;
};


class Dashboard : public sf::Drawable {
public:
    static constexpr float DASHBOARD_WIDTH = 450.0f;
    static constexpr float DASHBOARD_HEIGHT = 700.0f;
    static constexpr float PADDING = 15.0f;
    
    Dashboard(float x, float y, const sf::Font& font)
        : m_position(x, y)
        , m_font(font)
        , m_graph(std::make_unique<Graph>(x + PADDING, y + 80, DASHBOARD_WIDTH - 2 * PADDING, 200))
        , m_generationInput(x + 140, y + 300, 120, 28, 1, 1, 1)
        , m_trainCountInput(x + 180, y + 45, 100, 28, 20, 1, 1000)
        , m_startButton(x + 290, y + 45, 60, 28, "Start")
        , m_stopButton(x + 355, y + 45, 55, 28, "Stop")
    {
        // Background panel
        m_background.setPosition({x, y});
        m_background.setSize({DASHBOARD_WIDTH, DASHBOARD_HEIGHT});
        m_background.setFillColor(Constants::COLOR_ZINC_800);
        m_background.setOutlineColor(Constants::COLOR_ZINC_600);
        m_background.setOutlineThickness(2.0f);
        
        // Setup graph
        m_graph->setFont(font);
        m_graph->setTitle("Fitness over Generations");
        m_graph->setAxisLabels("Generation", "Fitness (s)");
        m_graph->setLineColor(Constants::COLOR_EMERALD_400);     // best: emerald
        m_graph->setLine2Color(sf::Color(100, 150, 200));        // avg: blue
        
        // Setup inputs
        m_generationInput.setFont(font);
        m_trainCountInput.setFont(font);
        
        // Setup buttons
        m_startButton.setFont(font);
        m_startButton.setColors(Constants::COLOR_EMERALD_600, Constants::COLOR_EMERALD_500, Constants::COLOR_EMERALD_400);
        
        m_stopButton.setFont(font);
        m_stopButton.setColors(Constants::COLOR_ROSE_500, Constants::COLOR_ROSE_400, sf::Color(255, 130, 150));
        m_stopButton.setEnabled(false);
        
        // Agent list area
        m_agentListBounds = sf::FloatRect(
            {x + PADDING, y + 345},
            {DASHBOARD_WIDTH - 2 * PADDING, DASHBOARD_HEIGHT - 345 - PADDING}
        );
    }
    
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }
    void toggleVisible() { m_visible = !m_visible; }
    
    void setFullscreen(bool fullscreen) {
        if (m_fullscreen == fullscreen) return;
        m_fullscreen = fullscreen;

        // mark layout dirty
        m_layoutDirty = true;
    }
    bool isFullscreen() const { return m_fullscreen; }
    
    void updateLayoutIfNeeded() {
        if (m_layoutDirty) {
            updateLayout();
            m_layoutDirty = false;
        }
    }
    
    // Update graph data from training history
    void updateFromHistory(const TrainingHistory& history) {
        if (history.isEmpty()) {
            m_graph->clearData();
            return;
        }
        
        m_graph->setData(history.getBestFitnessHistory());
        m_graph->setData2(history.getAvgFitnessHistory());
        
        m_generationInput.setRange(
            history.getFirstGenerationNumber(),
            history.getLastGenerationNumber()
        );
    }
    
    // Update agent list from generation snapshot
    void updateAgentList(const GenerationSnapshot* snapshot, int selectedAgentIndex) {
        m_agentItems.clear();
        if (!snapshot) return;
        
        int rank = 1;
        float bestFitness = snapshot->agentRecords.empty() ? 0.0f : snapshot->agentRecords[0].fitness;
        
        for (const auto& record : snapshot->agentRecords) {
            AgentListItem item;
            item.originalIndex = record.originalIndex;
            item.rank = rank++;
            item.fitness = record.fitness;
            item.isSelected = (record.originalIndex == selectedAgentIndex);
            item.isBest = (record.fitness == bestFitness && rank == 2); // First item
            m_agentItems.push_back(item);
        }
    }
    
    int getTrainCount() const { return m_trainCountInput.getValue(); }
    int getSelectedGeneration() const { return m_generationInput.getValue(); }
    
    void setSelectedGeneration(int gen) {
        m_generationInput.setValue(gen);
    }
    
    void setTrainingState(bool isTraining) {
        m_isTraining = isTraining;
        m_startButton.setEnabled(!isTraining);
        m_stopButton.setEnabled(isTraining);
    }
    
    void setOnStartTraining(std::function<void(int)> callback) {
        m_onStartTraining = callback;
        m_startButton.setOnClick([this]() {
            if (m_onStartTraining) {
                m_onStartTraining(m_trainCountInput.getValue());
            }
        });
    }
    
    void setOnStopTraining(std::function<void()> callback) {
        m_onStopTraining = callback;
        m_stopButton.setOnClick([this]() {
            if (m_onStopTraining) {
                m_onStopTraining();
            }
        });
    }
    
    void setOnAgentSelected(std::function<void(int)> callback) {
        m_onAgentSelected = callback;
    }
    
    void setOnGenerationChanged(std::function<void(int)> callback) {
        m_onGenerationChanged = callback;
    }
    
    // Event handling
    bool handleClick(float x, float y) {
        if (!m_visible) return false;
        if (!m_background.getGlobalBounds().contains({x, y})) return false;
        
        m_generationInput.stopEditing();
        m_trainCountInput.stopEditing();
        
        if (m_startButton.contains(x, y)) {
            m_startButton.click();
            return true;
        }
        if (m_stopButton.contains(x, y)) {
            m_stopButton.click();
            return true;
        }
        
        if (m_trainCountInput.handleClick(x, y)) return true;
        
        int oldGen = m_generationInput.getValue();
        if (m_generationInput.handleClick(x, y)) {
            if (m_generationInput.getValue() != oldGen && m_onGenerationChanged) {
                m_onGenerationChanged(m_generationInput.getValue());
            }
            return true;
        }
        
        if (m_agentListBounds.contains({x, y})) {
            float relY = y - m_agentListBounds.position.y + m_scrollOffset;
            int itemIndex = static_cast<int>(relY / AGENT_ITEM_HEIGHT);
            
            if (itemIndex >= 0 && itemIndex < static_cast<int>(m_agentItems.size())) {
                if (m_onAgentSelected) {
                    m_onAgentSelected(m_agentItems[itemIndex].originalIndex);
                }
                return true;
            }
        }
        
        return true;
    }
    
    void handleMouseMove(float x, float y) {
        if (!m_visible) return;
        
        m_startButton.setHovered(m_startButton.contains(x, y));
        m_stopButton.setHovered(m_stopButton.contains(x, y));
        m_generationInput.handleMouseMove(x, y);
        m_trainCountInput.handleMouseMove(x, y);
        
        m_hoveredAgentIndex = -1;
        if (m_agentListBounds.contains({x, y})) {
            float relY = y - m_agentListBounds.position.y + m_scrollOffset;
            int itemIndex = static_cast<int>(relY / AGENT_ITEM_HEIGHT);
            if (itemIndex >= 0 && itemIndex < static_cast<int>(m_agentItems.size())) {
                m_hoveredAgentIndex = itemIndex;
            }
        }
    }
    
    void handleScroll(float delta) {
        if (!m_visible) return;
        
        float maxScroll = std::max(0.0f, m_agentItems.size() * AGENT_ITEM_HEIGHT - m_agentListBounds.size.y);
        m_scrollOffset = std::clamp(m_scrollOffset - delta * 30.0f, 0.0f, maxScroll);
    }
    
    bool handleTextInput(char c) {
        if (!m_visible) return false;
        
        if (m_generationInput.handleTextInput(c)) {
            return true;
        }
        if (m_trainCountInput.handleTextInput(c)) {
            return true;
        }
        return false;
    }
    
    bool handleKey(sf::Keyboard::Key key) {
        if (!m_visible) return false;
        
        if (!m_generationInput.isEditing() && !m_trainCountInput.isEditing()) {
            if (key == sf::Keyboard::Key::Left) {
                int oldGen = m_generationInput.getValue();
                m_generationInput.decrement();
                if (m_generationInput.getValue() != oldGen && m_onGenerationChanged) {
                    m_onGenerationChanged(m_generationInput.getValue());
                }
                return true;
            }
            if (key == sf::Keyboard::Key::Right) {
                int oldGen = m_generationInput.getValue();
                m_generationInput.increment();
                if (m_generationInput.getValue() != oldGen && m_onGenerationChanged) {
                    m_onGenerationChanged(m_generationInput.getValue());
                }
                return true;
            }
        }
        
        if (m_generationInput.handleKey(key)) {
            if (key == sf::Keyboard::Key::Enter && m_onGenerationChanged) {
                m_onGenerationChanged(m_generationInput.getValue());
            }
            return true;
        }
        if (m_trainCountInput.handleKey(key)) {
            return true;
        }
        
        return false;
    }

private:
    static constexpr float AGENT_ITEM_HEIGHT = 24.0f;
    
    void updateLayout() {

        std::vector<std::pair<int, float>> savedData, savedData2;
        if (m_graph) {
            savedData = m_graph->getData();
            savedData2 = m_graph->getData2();
        }
        
        if (m_fullscreen) {

            m_background.setPosition({0, 0});
            m_background.setSize({Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT});
            
            // Recreate graph
            m_graph = std::make_unique<Graph>(PADDING, 80, Constants::WINDOW_WIDTH - 2 * PADDING - 20, 400);
            m_graph->setFont(m_font);
            m_graph->setTitle("Fitness over Generations");
            m_graph->setAxisLabels("Generation", "Fitness (s)");
            m_graph->setLineColor(Constants::COLOR_EMERALD_400);
            m_graph->setLine2Color(sf::Color(100, 150, 200));
            
            // Reposition controls
            m_generationInput = NumberInput(140, 500, 120, 28, m_generationInput.getValue(), 
                                          m_generationInput.getMinValue(), m_generationInput.getMaxValue());
            m_generationInput.setFont(m_font);
            
            m_trainCountInput = NumberInput(180, 45, 100, 28, m_trainCountInput.getValue(), 1, 1000);
            m_trainCountInput.setFont(m_font);
            
            m_startButton = Button(290, 45, 60, 28, "Start");
            m_startButton.setFont(m_font);
            m_startButton.setColors(Constants::COLOR_EMERALD_600, Constants::COLOR_EMERALD_500, Constants::COLOR_EMERALD_400);
            m_startButton.setEnabled(!m_isTraining);
            
            m_stopButton = Button(355, 45, 55, 28, "Stop");
            m_stopButton.setFont(m_font);
            m_stopButton.setColors(Constants::COLOR_ROSE_500, Constants::COLOR_ROSE_400, sf::Color(255, 130, 150));
            m_stopButton.setEnabled(m_isTraining);
            
            reconnectCallbacks();
            
            // Agent list area
            m_agentListBounds = sf::FloatRect(
                {PADDING, 545},
                {1550, 200}
            );
            
            m_position = {0, 0};
        } else {
            float dashboardX = Constants::WINDOW_WIDTH - DASHBOARD_WIDTH - 10.0f;
            float dashboardY = 30.0f;
            
            m_background.setPosition({dashboardX, dashboardY});
            m_background.setSize({DASHBOARD_WIDTH, DASHBOARD_HEIGHT});
            
            m_graph = std::make_unique<Graph>(dashboardX + PADDING, dashboardY + 80, DASHBOARD_WIDTH - 2 * PADDING, 200);
            m_graph->setFont(m_font);
            m_graph->setTitle("Fitness over Generations");
            m_graph->setAxisLabels("Generation", "Fitness (s)");
            m_graph->setLineColor(Constants::COLOR_EMERALD_400);
            m_graph->setLine2Color(sf::Color(100, 150, 200));
            
            m_generationInput = NumberInput(dashboardX + 140, dashboardY + 300, 120, 28, 
                                          m_generationInput.getValue(), m_generationInput.getMinValue(), 
                                          m_generationInput.getMaxValue());
            m_generationInput.setFont(m_font);
            
            m_trainCountInput = NumberInput(dashboardX + 180, dashboardY + 45, 100, 28, 
                                          m_trainCountInput.getValue(), 1, 1000);
            m_trainCountInput.setFont(m_font);
            
            m_startButton = Button(dashboardX + 290, dashboardY + 45, 60, 28, "Start");
            m_startButton.setFont(m_font);
            m_startButton.setColors(Constants::COLOR_EMERALD_600, Constants::COLOR_EMERALD_500, Constants::COLOR_EMERALD_400);
            m_startButton.setEnabled(!m_isTraining);
            
            m_stopButton = Button(dashboardX + 355, dashboardY + 45, 55, 28, "Stop");
            m_stopButton.setFont(m_font);
            m_stopButton.setColors(Constants::COLOR_ROSE_500, Constants::COLOR_ROSE_400, sf::Color(255, 130, 150));
            m_stopButton.setEnabled(m_isTraining);
            
            reconnectCallbacks();
            
            m_agentListBounds = sf::FloatRect(
                {dashboardX + PADDING, dashboardY + 345},
                {DASHBOARD_WIDTH - 2 * PADDING, DASHBOARD_HEIGHT - 345 - PADDING}
            );
            
            m_position = {dashboardX, dashboardY};
        }
        
        if (m_graph && (!savedData.empty() || !savedData2.empty())) {
            m_graph->setData(savedData);
            m_graph->setData2(savedData2);
        }
    }
    
    void reconnectCallbacks() {
        if (m_onStartTraining) {
            m_startButton.setOnClick([this]() {
                if (m_onStartTraining) {
                    m_onStartTraining(m_trainCountInput.getValue());
                }
            });
        }
        if (m_onStopTraining) {
            m_stopButton.setOnClick([this]() {
                if (m_onStopTraining) {
                    m_onStopTraining();
                }
            });
        }
    }
    
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (!m_visible) return;
        
        const_cast<Dashboard*>(this)->updateLayoutIfNeeded();
        
        target.draw(m_background, states);
        
        // Titl
        sf::Text title(m_font, "Training Dashboard", 18);
        title.setFillColor(Constants::COLOR_EMERALD_400);
        title.setStyle(sf::Text::Bold);
        title.setPosition({m_position.x + PADDING, m_position.y + PADDING});
        target.draw(title, states);
        
        // Training control label
        sf::Text trainLabel(m_font, "Train for:", 13);
        trainLabel.setFillColor(Constants::COLOR_ZINC_400);
        trainLabel.setPosition({m_position.x + PADDING, m_position.y + 50});
        target.draw(trainLabel, states);
        
        sf::Text genLabel(m_font, "generations", 13);
        genLabel.setFillColor(Constants::COLOR_ZINC_400);
        genLabel.setPosition({m_position.x + 95, m_position.y + 50});
        target.draw(genLabel, states);
        
        // inputs and buttons
        target.draw(m_trainCountInput, states);
        target.draw(m_startButton, states);
        target.draw(m_stopButton, states);
        
        if (m_graph) target.draw(*m_graph, states);
        
        // Generation selector section
        float genSelectorY = m_fullscreen ? 505 : 305;
        sf::Text genSelectLabel(m_font, "View Generation:", 13);
        genSelectLabel.setFillColor(Constants::COLOR_ZINC_400);
        genSelectLabel.setPosition({m_position.x + PADDING, m_position.y + genSelectorY});
        target.draw(genSelectLabel, states);
        
        target.draw(m_generationInput, states);
        
        // Legend
        float legendY = m_fullscreen ? 503 : 303;
        float legendBaseY = m_fullscreen ? 500 : 300;
        sf::CircleShape bestDot(5.0f);
        bestDot.setFillColor(Constants::COLOR_EMERALD_400);
        bestDot.setPosition({m_position.x + 270, m_position.y + legendY});
        target.draw(bestDot, states);
        
        sf::Text bestLegend(m_font, "Best", 11);
        bestLegend.setFillColor(Constants::COLOR_ZINC_400);
        bestLegend.setPosition({m_position.x + 283, m_position.y + legendBaseY});
        target.draw(bestLegend, states);
        
        sf::CircleShape avgDot(5.0f);
        avgDot.setFillColor(sf::Color(100, 150, 200));
        avgDot.setPosition({m_position.x + 320, m_position.y + legendY});
        target.draw(avgDot, states);
        
        sf::Text avgLegend(m_font, "Avg", 11);
        avgLegend.setFillColor(Constants::COLOR_ZINC_400);
        avgLegend.setPosition({m_position.x + 333, m_position.y + legendBaseY});
        target.draw(avgLegend, states);
        
        // Agent list header
        float agentHeaderY = m_fullscreen ? 530 : 330;
        sf::Text agentHeader(m_font, "Agents (sorted by fitness)", 14);
        agentHeader.setFillColor(Constants::COLOR_ZINC_300);
        agentHeader.setStyle(sf::Text::Bold);
        agentHeader.setPosition({m_position.x + PADDING, m_position.y + agentHeaderY});
        target.draw(agentHeader, states);
        
        drawAgentList(target, states);
    }
    
    void drawAgentList(sf::RenderTarget& target, sf::RenderStates states) const {
        sf::View oldView = target.getView();
        
        sf::View clipView;
        clipView.setCenter({
            m_agentListBounds.position.x + m_agentListBounds.size.x / 2.0f,
            m_agentListBounds.position.y + m_agentListBounds.size.y / 2.0f
        });
        clipView.setSize({m_agentListBounds.size.x, m_agentListBounds.size.y});
        
        sf::Vector2u windowSize = target.getSize();
        float vpLeft = m_agentListBounds.position.x / windowSize.x;
        float vpTop = m_agentListBounds.position.y / windowSize.y;
        float vpWidth = m_agentListBounds.size.x / windowSize.x;
        float vpHeight = m_agentListBounds.size.y / windowSize.y;
        
        clipView.setViewport(sf::FloatRect({vpLeft, vpTop}, {vpWidth, vpHeight}));
        target.setView(clipView);
        
        float y = m_agentListBounds.position.y - m_scrollOffset;
        
        for (size_t i = 0; i < m_agentItems.size(); ++i) {
            const auto& item = m_agentItems[i];
            
            if (y + AGENT_ITEM_HEIGHT < m_agentListBounds.position.y || 
                y > m_agentListBounds.position.y + m_agentListBounds.size.y) {
                y += AGENT_ITEM_HEIGHT;
                continue;
            }
            
            sf::RectangleShape itemBg;
            itemBg.setPosition({m_agentListBounds.position.x, y});
            itemBg.setSize({m_agentListBounds.size.x, AGENT_ITEM_HEIGHT - 2});
            
            if (item.isSelected) {
                itemBg.setFillColor(sf::Color(Constants::COLOR_EMERALD_600.r, Constants::COLOR_EMERALD_600.g, 
                                              Constants::COLOR_EMERALD_600.b, 150));
            } else if (static_cast<int>(i) == m_hoveredAgentIndex) {
                itemBg.setFillColor(sf::Color(Constants::COLOR_ZINC_700.r, Constants::COLOR_ZINC_700.g,
                                              Constants::COLOR_ZINC_700.b, 150));
            } else {
                itemBg.setFillColor(sf::Color(0, 0, 0, 0));
            }
            target.draw(itemBg, states);
            
            // Agent text
            std::string text = "#" + std::to_string(item.rank) + "  Agent " + 
                               std::to_string(item.originalIndex) + "  -  " + 
                               std::to_string(static_cast<int>(item.fitness));
            
            sf::Text itemText(m_font, text, 12);
            itemText.setFillColor(item.isSelected ? Constants::COLOR_ZINC_300 : Constants::COLOR_ZINC_400);
            itemText.setPosition({m_agentListBounds.position.x + 5.0f, y + 3.0f});
            target.draw(itemText, states);
            
            y += AGENT_ITEM_HEIGHT;
        }
        
        // Restore view
        target.setView(oldView);
        
        // Draw scroll indicators
        float maxScroll = std::max(0.0f, m_agentItems.size() * AGENT_ITEM_HEIGHT - m_agentListBounds.size.y);
        if (maxScroll > 0) {
            float scrollRatio = m_scrollOffset / maxScroll;
            float scrollBarHeight = std::max(20.0f, m_agentListBounds.size.y * (m_agentListBounds.size.y / (m_agentItems.size() * AGENT_ITEM_HEIGHT)));
            float scrollBarY = m_agentListBounds.position.y + scrollRatio * (m_agentListBounds.size.y - scrollBarHeight);
            
            sf::RectangleShape scrollBar;
            scrollBar.setPosition({m_agentListBounds.position.x + m_agentListBounds.size.x - 6, scrollBarY});
            scrollBar.setSize({4, scrollBarHeight});
            scrollBar.setFillColor(Constants::COLOR_ZINC_600);
            target.draw(scrollBar, states);
        }
    }
    
    sf::Vector2f m_position;
    const sf::Font& m_font;
    bool m_visible = false;
    bool m_isTraining = false;
    bool m_fullscreen = false;
    bool m_layoutDirty = false;
    
    sf::RectangleShape m_background;
    std::unique_ptr<Graph> m_graph;
    
    NumberInput m_generationInput;
    NumberInput m_trainCountInput;
    Button m_startButton;
    Button m_stopButton;
    
    sf::FloatRect m_agentListBounds;
    std::vector<AgentListItem> m_agentItems;
    float m_scrollOffset = 0.0f;
    mutable int m_hoveredAgentIndex = -1;
    
    // Callbacks
    std::function<void(int)> m_onStartTraining;
    std::function<void()> m_onStopTraining;
    std::function<void(int)> m_onAgentSelected;
    std::function<void(int)> m_onGenerationChanged;
};
