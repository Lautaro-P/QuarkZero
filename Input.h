#pragma once

enum UIAction
{
    ACTION_NONE,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_ENTER,
    ACTION_BACK
};

class Input
{
private:
    UIAction lastDir = ACTION_NONE;
    unsigned long lastTime = 0;
    const unsigned long repeatDelay = 700; // ms
    const unsigned long repeatRate = 400;  // ms

public:
    Input();
    UIAction handle();
};