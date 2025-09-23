#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>
#include "Input.h"

#define NAVBAR_HEIGHT 11

// --- Icon ---
class Icon
{
public:
    String label;
    const uint8_t *bitmap;
    int w, h;

    Icon(String id, const uint8_t *bmp, int width, int height)
        : label(id), bitmap(bmp), w(width), h(height) {}
};

// --- Body abstracto ---
class Body
{
public:
    virtual void render(Adafruit_SSD1306 &disp, int x, int y, int w, int h) = 0;
    virtual void handleInput(UIAction a) {}
    virtual ~Body() {}
};

// --- NavBar ---
class NavBar
{
public:
    String title;
    std::vector<Icon *> icons;

    void render(Adafruit_SSD1306 &disp, int width, int height);

    // Gestión de íconos
    void setIcon(Icon *icon);
    void removeIcon(const String &id);
};

struct Item
{
    String label;
    Icon *icon = nullptr;
    std::function<void()> callback = nullptr;

    // Constructor con todos los parámetros opcionales
    Item(const String &l, Icon *i = nullptr, std::function<void()> cb = nullptr)
        : label(l), icon(i), callback(cb) {}

    Item() = default; // constructor por defecto
};

class ListBody : public Body
{
public:
    std::vector<Item> items;
    int currentIndex = 0;
    int topIndex = 0;
    int maxVisible = 4;
    float scrollOffset = 0;
    float targetOffset = 0;
    float velScroll = 0.3;
    float currentPosY = 0;

    void render(Adafruit_SSD1306 &disp, int x, int y, int w, int h) override;
    void handleInput(UIAction a) override;
    void up();
    void down();
};

// --- WindowManager ---
class WindowManager
{
private:
    std::vector<Body *> stack;

public:
    NavBar navbar;

    void push(Body *next);
    void pop();
    bool canGoBack();
    Body *current();
    void render(Adafruit_SSD1306 &disp);
};

// --- Funciones auxiliares ---
void invertRect(Adafruit_SSD1306 &disp, int16_t x, int16_t y, int16_t w, int16_t h);
