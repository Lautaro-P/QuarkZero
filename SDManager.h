#pragma once
#include <Arduino.h>
#include <SD.h>
#include "UI.h"
#include "Icons.h"

class SDManager {
public:
    SDManager(int csPin, WindowManager &windowManager);

    bool begin();
    bool isInserted();

    void loadFolder(ListBody &menu, const char *path);

private:
    int _csPin;
    bool _mounted;
    WindowManager &wm;
    bool _lastState; 
};
