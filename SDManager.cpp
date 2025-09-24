#include "SDManager.h"

SDManager::SDManager(int csPin, WindowManager &windowManager)
: _csPin(csPin), _mounted(false), wm(windowManager) {}

bool SDManager::begin() {
    if (!SD.begin(_csPin)) {
        Serial.println("❌ No se pudo montar la tarjeta SD");
        wm.navbar.setIcon(&iconSDFail);
        _mounted = false;
        return false;
    }
    Serial.println("✅ SD montada correctamente");
    wm.navbar.setIcon(&iconSDMounted);
    _mounted = true;
    return true;
}

bool SDManager::isInserted() {
    return _mounted;
}

void SDManager::loadFolder(ListBody &menu, const char *path) {
    menu.items.clear();

    menu.items.push_back({"..", &iconBack, [this]()
                          {
                              wm.pop();
                          }});

    menu.currentIndex = 0;

    File root = SD.open(path);
    if (!root || !root.isDirectory()) {
        Serial.println("No es carpeta");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        String name = file.name();
        if (file.isDirectory()) {
            menu.items.push_back({name, &iconFolder, [this, &menu, name]()
                                  {
                                      this->loadFolder(menu, name.c_str());
                                      wm.push(&menu);
                                  }});
        } else {
            menu.items.push_back({name, &iconFile, [name]()
                                  {
                                      Serial.printf("Abrir archivo: %s\n", name.c_str());
                                  }});
        }
        file = root.openNextFile();
    }
}
