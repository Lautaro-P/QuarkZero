#include "Input.h"
#include "Arduino.h"

#define PIN_UP 37
#define PIN_DOWN 38
#define PIN_LEFT 39
#define PIN_RIGHT 40

Input::Input()
{
    pinMode(PIN_UP, INPUT_PULLUP);
    pinMode(PIN_DOWN, INPUT_PULLUP);
    pinMode(PIN_LEFT, INPUT_PULLUP);
    pinMode(PIN_RIGHT, INPUT_PULLUP);
}

UIAction Input::handle()
{
    unsigned long now = millis();

    // --- Si no hay ENTER/BACK, procesamos direcciones ---
    int dx = 0;
    int dy = 0;

    if (digitalRead(PIN_LEFT) == LOW)  dx -= 1;
    if (digitalRead(PIN_RIGHT) == LOW) dx += 1;
    if (digitalRead(PIN_UP) == LOW)    dy -= 1;
    if (digitalRead(PIN_DOWN) == LOW)  dy += 1;

    UIAction dir = ACTION_NONE;
    if (dx < 0)      dir = ACTION_LEFT;
    else if (dx > 0) dir = ACTION_RIGHT;
    else if (dy < 0) dir = ACTION_UP;
    else if (dy > 0) dir = ACTION_DOWN;

    // Si dirección cambió
    if (dir != lastDir)
    {
        if ((now - lastTime) >= (repeatRate / 2))
        {
            lastDir = dir;
            lastTime = now;

            return dir;
        }
        else
        {
            return ACTION_NONE; // cambio muy rápido → ruido
        }
    }

    // Si está en reposo
    if (dir == ACTION_NONE)
    {
        lastDir = ACTION_NONE;
        return ACTION_NONE;
    }

    // Si mantiene la misma dirección
    if (dir == lastDir)
    {
        unsigned long elapsed = now - lastTime;
        if (elapsed >= repeatDelay)
        {
            lastTime = now - (repeatDelay - repeatRate);
            return dir;
        }
    }

    return ACTION_NONE;
}
