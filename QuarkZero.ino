#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

#include "UI.h"
#include "Input.h"
#include "Icons.h"
#include "SDManager.h"

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
SDManager sdManager(PIN_SD_CS, wm);

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

  if (!sdManager.begin()) {
    Serial.println("No hay SD al inicio");
  }

  if (sdManager.isInserted()) {
    menu.items.push_back({"SD", &iconSD, []()
       {
         sdManager.loadFolder(sdMenu, "/");
         wm.push(&sdMenu);
       }});
  }

  menu.items = {
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
