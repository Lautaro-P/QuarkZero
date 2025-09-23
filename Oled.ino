#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

#include "UI.h"
#include "Input.h"

#define PIN_SD_CS 10

#define SCREEN_WIDTH 128 // OLED display width, in pixel
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WindowManager wm;
ListBody menu;
ListBody sdMenu;

static const unsigned char PROGMEM image_battEmpty[] = {0x00, 0x00, 0x3f, 0xc0, 0x40, 0x20, 0xc0, 0x20, 0xc0, 0x20, 0x40, 0x20, 0x3f, 0xc0, 0x00, 0x00};
static const unsigned char PROGMEM image_battLow[] = {0x00, 0x00, 0x3f, 0xc0, 0x40, 0x20, 0xc0, 0xa0, 0xc0, 0xa0, 0x40, 0x20, 0x3f, 0xc0, 0x00, 0x00};
static const unsigned char PROGMEM image_battMid[] = {0x00, 0x00, 0x3f, 0xc0, 0x40, 0x20, 0xc2, 0xa0, 0xc2, 0xa0, 0x40, 0x20, 0x3f, 0xc0, 0x00, 0x00};
static const unsigned char PROGMEM image_battFull[] = {0x00, 0x00, 0x3f, 0xc0, 0x40, 0x20, 0xca, 0xa0, 0xca, 0xa0, 0x40, 0x20, 0x3f, 0xc0, 0x00, 0x00};
static const unsigned char PROGMEM image_SDcardFail_bits[] = {0xff, 0xe0, 0xed, 0xe0, 0xff, 0xe0, 0xe1, 0xe0, 0xde, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 0xe6, 0x00};
static const unsigned char PROGMEM image_SDcardMounted_bits[] = {0xff, 0xe0, 0xff, 0x20, 0xff, 0xe0, 0xff, 0x20, 0xff, 0xe0, 0xff, 0x20, 0xff, 0xe0, 0xe6, 0x00};
static const unsigned char PROGMEM image_Bluetooth_Idle_bits[] = {0x20, 0xb0, 0x68, 0x30, 0x30, 0x68, 0xb0, 0x20};
static const unsigned char PROGMEM image_Alert_bits[] = {0x08, 0x00, 0x1c, 0x00, 0x14, 0x00, 0x36, 0x00, 0x36, 0x00, 0x7f, 0x00, 0x77, 0x00, 0xff, 0x80};
static const unsigned char PROGMEM image_GameMode_bits[] = {0x04, 0x00, 0x7f, 0xc0, 0xdf, 0xe0, 0x8e, 0xa0, 0xdf, 0xe0, 0xf1, 0xe0, 0xe0, 0xe0, 0xc0, 0x60};
static const unsigned char PROGMEM image_folder[] = {0x00, 0x00, 0x70, 0x00, 0x5f, 0x80, 0x7f, 0x80, 0x40, 0x80, 0x40, 0x80, 0x40, 0x80, 0x7f, 0x80, 0x00, 0x00};
static const unsigned char PROGMEM image_file[] = {0x00, 0x00, 0x7c, 0x00, 0x46, 0x00, 0x47, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00, 0x7f, 0x00, 0x00, 0x00};
static const unsigned char PROGMEM image_backArrow[] = {0x00, 0x00, 0xf0, 0xe0, 0xf1, 0xbe, 0x18, 0x00, 0x00};
static const unsigned char PROGMEM image_SDcardMounted_Menu []  = {0xff, 0xe0, 0xff, 0x20, 0xff, 0xe0, 0xff, 0x20, 0xff, 0x20, 0xff, 0xe0, 0xe6, 0x00};

Icon iconBack("backIcon", image_backArrow, 8, 9);
Icon iconMore("sdItemIcon", image_SDcardMounted_Menu, 11, 7);
Icon iconBattery("battery", image_battEmpty, 11, 8);
Icon iconBatteryLow("battery", image_battLow, 11, 8);
Icon iconBatteryMid("battery", image_battMid, 11, 8);
Icon iconBatteryFull("battery", image_battFull, 11, 8);
Icon iconSDFail("sd", image_SDcardFail_bits, 11, 8);
Icon iconSDMounted("sd", image_SDcardMounted_bits, 11, 8);
Icon iconAlert("alert", image_Alert_bits, 9, 8);
Icon iconBluetooth("bluetooth", image_Bluetooth_Idle_bits, 5, 8);
Icon iconGameMode("gamemode", image_GameMode_bits, 11, 8);
Icon iconFolder("folder", image_folder, 10, 9);
Icon iconFile("file", image_file, 9, 9);

Input input;

void loadFolder(ListBody &menu, const char *path) {
  menu.items.clear();

  menu.items.push_back({"..", &iconBack, []() {
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
      menu.items.push_back({name, &iconFolder, [name]() {
        loadFolder(sdMenu, name.c_str());
        wm.push(&sdMenu);
      }});
    } else {
      menu.items.push_back({name, &iconFile, [name]() {
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
  wm.navbar.title = "Menu";
  wm.navbar.setIcon(&iconBattery);
  wm.navbar.setIcon(&iconSDFail);

  menu.items = {
      {"Iniciar"},
      {"Ajustes"},
      {"SD", &iconFolder, []() {
        loadFolder(sdMenu, "/");
        wm.push(&sdMenu);
      }},
      {"Salir", nullptr, []()
       { Serial.println("Salir"); }}
  };

  wm.push(&menu);
  
  wm.navbar.setIcon(&iconBatteryFull);
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("SD Mount Failed");
    wm.navbar.setIcon(&iconSDFail);
  } else {
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
