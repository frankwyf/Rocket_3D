// UI System for SkyRocket 3D
// Provides HUD and menu interfaces

#ifndef UI_SYSTEM_HPP
#define UI_SYSTEM_HPP

#include <string>
#include <vector>
#include <functional>

#include "../vmlib/vec2.hpp"
#include "../vmlib/vec4.hpp"

// ============================================================================
// UI Element Base Classes
// ============================================================================

struct UIColor
{
    float r, g, b, a;
    UIColor(float r_ = 1.0f, float g_ = 1.0f, float b_ = 1.0f, float a_ = 1.0f)
        : r(r_), g(g_), b(b_), a(a_) {}
};

class UIElement
{
public:
    virtual ~UIElement() = default;

    virtual void render() = 0;
    virtual void update(float deltaTime) = 0;
    virtual bool handleInput(int key, int action) { return false; }

    void setPosition(float x, float y) { x_ = x; y_ = y; }
    void setSize(float w, float h) { width_ = w; height_ = h; }
    void setColor(const UIColor& color) { color_ = color; }

    float getX() const { return x_; }
    float getY() const { return y_; }
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }

protected:
    float x_ = 0.0f;
    float y_ = 0.0f;
    float width_ = 0.1f;
    float height_ = 0.1f;
    UIColor color_;
    bool visible_ = true;
};

// ============================================================================
// HUD (Heads-Up Display)
// ============================================================================

class HUD : public UIElement
{
public:
    HUD() : score_(0), fuel_(100.0f), altitude_(0.0f), speed_(0.0f) {}

    void render() override;
    void update(float deltaTime) override;

    // Update HUD values
    void setScore(int score) { score_ = score; }
    void setFuel(float fuel) { fuel_ = fuel; }
    void setAltitude(float altitude) { altitude_ = altitude; }
    void setSpeed(float speed) { speed_ = speed; }
    void setLandingAccuracy(int accuracy) { landingAccuracy_ = accuracy; }

    int getScore() const { return score_; }
    float getFuel() const { return fuel_; }
    float getAltitude() const { return altitude_; }
    float getSpeed() const { return speed_; }

private:
    int score_;
    float fuel_;
    float altitude_;
    float speed_;
    int landingAccuracy_ = 0;
};

// ============================================================================
// Menu System
// ============================================================================

class MenuItem
{
public:
    typedef std::function<void()> Callback;

    MenuItem(const std::string& label, Callback callback)
        : label_(label), callback_(callback), selected_(false) {}

    const std::string& getLabel() const { return label_; }
    void invoke() { if (callback_) callback_(); }
    void setSelected(bool selected) { selected_ = selected; }
    bool isSelected() const { return selected_; }

private:
    std::string label_;
    Callback callback_;
    bool selected_;
};

class Menu : public UIElement
{
public:
    void render() override;
    void update(float deltaTime) override;
    bool handleInput(int key, int action) override;

    void addItem(const MenuItem& item) { items_.push_back(item); }
    void selectNext();
    void selectPrevious();
    void selectCurrent();
    void clear() { items_.clear(); }

private:
    std::vector<MenuItem> items_;
    int selectedIndex_ = 0;
};

// ============================================================================
// Mini-Map UI Element
// ============================================================================

class MiniMap : public UIElement
{
public:
    MiniMap() : playerX_(0.0f), playerY_(0.0f), rotation_(0.0f) {}

    void render() override;
    void update(float deltaTime) override;

    void setPlayerPosition(float x, float z) { playerX_ = x; playerY_ = z; }
    void setRotation(float angle) { rotation_ = angle; }
    void addWaypoint(float x, float z);

private:
    float playerX_, playerY_;
    float rotation_;
    std::vector<Vec2f> waypoints_;
};

// ============================================================================
// Damage Indicator
// ============================================================================

class DamageIndicator : public UIElement
{
public:
    DamageIndicator() : displayTime_(0.0f), damageAmount_(0) {}

    void render() override;
    void update(float deltaTime) override;

    void showDamage(int amount)
    {
        damageAmount_ = amount;
        displayTime_ = 2.0f; // Show for 2 seconds
    }

private:
    float displayTime_;
    int damageAmount_;
};

// ============================================================================
// UI Manager
// ============================================================================

class UIManager
{
public:
    UIManager() : hud_(), menu_(), miniMap_(), damageIndicator_() {}

    void render();
    void update(float deltaTime);
    bool handleInput(int key, int action);

    HUD& getHUD() { return hud_; }
    Menu& getMenu() { return menu_; }
    MiniMap& getMiniMap() { return miniMap_; }
    DamageIndicator& getDamageIndicator() { return damageIndicator_; }

    void showMenu(bool show) { menuVisible_ = show; }
    bool isMenuVisible() const { return menuVisible_; }

private:
    HUD hud_;
    Menu menu_;
    MiniMap miniMap_;
    DamageIndicator damageIndicator_;
    bool menuVisible_ = false;
};

#endif // UI_SYSTEM_HPP
