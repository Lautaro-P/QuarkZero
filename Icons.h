#pragma once
#include <Arduino.h>
#include "UI.h"

// ===================== Battery =====================
extern const unsigned char PROGMEM image_battEmpty[];
extern const unsigned char PROGMEM image_battLow[];
extern const unsigned char PROGMEM image_battMid[];
extern const unsigned char PROGMEM image_battFull[];

// ===================== Navbar =====================
extern const unsigned char PROGMEM image_SDcardFail[];
extern const unsigned char PROGMEM image_SDcardMounted[];
extern const unsigned char PROGMEM image_Bluetooth[];

// ===================== Items (9x9) =====================
extern const unsigned char PROGMEM image_backArrow[];
extern const unsigned char PROGMEM image_file[];
extern const unsigned char PROGMEM image_Folder[];
extern const unsigned char PROGMEM image_BT[];
extern const unsigned char PROGMEM image_SDcard[];
extern const unsigned char PROGMEM image_Config[];
extern const unsigned char PROGMEM image_IrLed[];
extern const unsigned char PROGMEM image_WiFi[];
extern const unsigned char PROGMEM image_NFC[];
extern const unsigned char PROGMEM image_USB[];

// ===================== Icon instances =====================
extern Icon iconBattery;
extern Icon iconBatteryLow;
extern Icon iconBatteryMid;
extern Icon iconBatteryFull;

extern Icon iconSDFail;
extern Icon iconSDMounted;
extern Icon iconBluetooth;

extern Icon iconBack;
extern Icon iconFile;
extern Icon iconFolder;
extern Icon iconBT;
extern Icon iconSD;
extern Icon iconConfig;
extern Icon iconIrLed;
extern Icon iconWiFi;
extern Icon iconNFC;
extern Icon iconUSB;
