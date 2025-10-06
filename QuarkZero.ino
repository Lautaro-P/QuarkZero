#include "RF24.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>
#include "esp_wifi.h"

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

int bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
int ble_channels[] = {2, 26, 80};

Input input;

constexpr int SPI_SPEED = 16000000;

// SDManager sdManager(PIN_SD_CS, wm);
RF24 radioVSPI(5, 12, SPI_SPEED);
RF24 radioHSPI(4, 13, SPI_SPEED);

void configureRadio(RF24 &radio, int channel, SPIClass *spi);

void setup()
{
  Serial.begin(115200);

  esp_wifi_stop();
  esp_wifi_deinit();
  esp_wifi_disconnect();

  while (!Serial)
  {
    delay(1);
  }
  configureRadio(radioVSPI, ble_channels[0], 5, 12);
  configureRadio(radioHSPI, bluetooth_channels[0], 4, 13);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  wm.navbar.title = "Inicio";

  // if (!sdManager.begin()) {
  //   Serial.println("No hay SD al inicio");
  // }

  // if (sdManager.isInserted()) {
  //   menu.items.push_back({"SD", &iconSD, []()
  //      {
  //        sdManager.loadFolder(sdMenu, "/");
  //        wm.push(&sdMenu);
  //      }});
  // }

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

void configureRadio(RF24 &radio, int channel, int CE, int CS) {
    if (radio.begin(CE, CS)) {
      Serial.print("Se inicio radio");
      Serial.println(CS);
      radio.setAutoAck(false);
      radio.stopListening();
      radio.setRetries(0, 0);
      radio.setPALevel(RF24_PA_MAX, true);
      radio.setDataRate(RF24_2MBPS);
      radio.setCRCLength(RF24_CRC_DISABLED);
      radio.startConstCarrier(RF24_PA_HIGH, channel);
    } else {
      Serial.print("NO se inicio radio");
      Serial.println(CS);
    }
}

void loop()
{
  UIAction action = input.handle();
  Body *current = wm.current();
  int randomIndex = random(0, sizeof(bluetooth_channels) / sizeof(bluetooth_channels[0]));
  int channel = bluetooth_channels[randomIndex];
  radioVSPI.setChannel(channel);
  radioHSPI.setChannel(channel);
  
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
