#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

#include "UI.h"
#include "Input.h"
#include "Icons.h"

#define PIN_SD_CS 10

#define SCREEN_WIDTH 128 // OLED display width, in pixel
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WindowManager wm;
ListBody menu;
ListBody sdMenu;

Input input;

void loadFolder(ListBody &menu, const char *path)
{
  menu.items.clear();

  menu.items.push_back({"..", &iconBack, []()
                        {
                          wm.pop();
                        }});

  menu.currentIndex = 0;

  File root = SD.open(path);
  if (!root || !root.isDirectory())
  {
    Serial.println("No es carpeta");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    String name = file.name();
    if (file.isDirectory())
    {
      menu.items.push_back({name, &iconFolder, [name]()
                            {
                              loadFolder(sdMenu, name.c_str());
                              wm.push(&sdMenu);
                            }});
    }
    else
    {
      menu.items.push_back({name, &iconFile, [name]()
                            {
                              Serial.printf("Abrir archivo: %s\n", name.c_str());
                            }});
    }
    file = root.openNextFile();
  }
}

void setup()
{
  Serial.begin(115200);

  Wire.begin(34, 33);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  wm.navbar.title = "Inicio";

  menu.items = {
      {"SD", &iconSD, []()
       {
         loadFolder(sdMenu, "/");
         wm.push(&sdMenu);
       }},
      {"Bluetooth", &iconBT, []()
       { Serial.println("BT"); }},
      {"IR", &iconIrLed, []()
       { Serial.println("IR"); }},
      {"WiFi", &iconWiFi, []()
       { Serial.println("WiFi"); }},
      {"BadUSB", &iconUSB, []()
       { Serial.println("USB"); }},
      {"NFC", &iconNFC, []()
       { Serial.println("NFC"); }},
      {"Config", &iconConfig, []()
       { Serial.println("CONFIG"); }},
  };

  wm.push(&menu);

  wm.navbar.setIcon(&iconBatteryFull);
  if (!SD.begin(PIN_SD_CS))
  {
    Serial.println("SD Mount Failed");
    wm.navbar.setIcon(&iconSDFail);
  }
  else
  {
    wm.navbar.setIcon(&iconSDMounted);
  }
  wm.render(display);
}

unsigned long lastUpdateInput = 0;
unsigned long lastUpdate = 0;
const int frameDelay = 20;
const int inputDelay = 400;

void loop()
{
  UIAction action = input.handle();
  Body *current = wm.current();

  if (action != ACTION_NONE)
  {
    Serial.printf("[%lu ms] Direccion: %i\n", millis(), action);
  }

  if (millis() - lastUpdateInput >= inputDelay)
  {
    current->handleInput(action);
  }

  if (millis() - lastUpdate >= frameDelay)
  {
    wm.render(display);
    lastUpdate = millis();
  }

  // delay(1);
}
