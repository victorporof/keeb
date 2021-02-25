#ifndef Bluetooth_h
#define Bluetooth_h

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
#include "Helpers.h"
#include "KeyCode.h"

#define FACTORYRESET_ENABLE 1
#define MINIMUM_FIRMWARE "0.7.0"

#define MODIFIER_BIT_LEFT_CONTROL 0x01
#define MODIFIER_BIT_LEFT_SHIFT 0x02
#define MODIFIER_BIT_LEFT_ALT 0x04
#define MODIFIER_BIT_LEFT_WINDOW 0x08
#define MODIFIER_BIT_RIGHT_CONTROL 0x10
#define MODIFIER_BIT_RIGHT_SHIFT 0x20
#define MODIFIER_BIT_RIGHT_ALT 0x40
#define MODIFIER_BIT_RIGHT_WINDOW 0x80

const int ASCII_CONV_TABLE[128][2] = HID_ASCII_TO_KEYCODE;

class Bluetooth_ {
  typedef struct {
    int modifiers;
    int keys[6];
  } KeyReport;

  KeyReport mReport{};
  Adafruit_BluefruitLE_SPI* mBle;

 public:
  Bluetooth_(int csPin, int spiPin, int rstPin) {
    mBle = new Adafruit_BluefruitLE_SPI(csPin, spiPin, rstPin);
  }

  ~Bluetooth_() { delete mBle; }

  void begin() {
    if (!mBle->begin(VERBOSE_MODE)) {
      error(F("Couldn't find Bluefruit."));
    }
    if (!mBle->sendCommandCheckOK(F("AT+BAUDRATE=9600"))) {
      error(F("Failed set baud rate."));
    }

    if (FACTORYRESET_ENABLE) {
      if (!mBle->factoryReset()) {
        error(F("Couldn't factory reset."));
      }
    }

    mBle->echo(false);
    mBle->info();

    if (!mBle->isVersionAtLeast(MINIMUM_FIRMWARE)) {
      error(F("Requires firmware version " MINIMUM_FIRMWARE " or higher."));
    }
    if (!mBle->sendCommandCheckOK(F("AT+GAPDEVNAME=" BLUETOOTH_DEVICE_NAME))) {
      error(F("Could not set device name."));
    }
    if (!mBle->sendCommandCheckOK(F("AT+BLEHIDEN=On"))) {
      error(F("Failed to enable HID."));
    }
    if (!mBle->reset()) {
      error(F("Could not reset."));
    }
  }

  bool mouseMove(int x, int y, int scroll) {
    char cmd[64];
    sprintf(cmd, "AT+BLEHIDMOUSEMOVE=%d,%d,%d,", x, y, scroll);
    return mBle->sendCommandCheckOK(cmd);
  }

  bool mousePress(int button) {
    if (button == MOUSE_LEFT) {
      return mBle->sendCommandCheckOK(F("AT+BLEHIDMOUSEBUTTON=L"));
    }
    if (button == MOUSE_MIDDLE) {
      return mBle->sendCommandCheckOK(F("AT+BLEHIDMOUSEBUTTON=M"));
    }
    if (button == MOUSE_RIGHT) {
      return mBle->sendCommandCheckOK(F("AT+BLEHIDMOUSEBUTTON=R"));
    }
    return false;
  }

  bool mouseRelease() {
    return mBle->sendCommandCheckOK(F("AT+BLEHIDMOUSEBUTTON=0"));
  }

  bool keyPress(char ch) {
    if (ch < 0 || ch >= 128) {
      return false;
    }
    int* pair = ASCII_CONV_TABLE[ch];
    int shift = pair[0];
    int key = pair[1];
    if (shift != 0) {
      hidKeyPress(HID_KEY_SHIFT_LEFT);
    }
    hidKeyPress(key);
    return true;
  }

  bool keyRelease(char ch) {
    if (ch < 0 || ch >= 128) {
      return false;
    }
    int* pair = ASCII_CONV_TABLE[ch];
    int shift = pair[0];
    int key = pair[1];
    if (shift != 0) {
      hidKeyRelease(HID_KEY_SHIFT_LEFT);
    }
    hidKeyRelease(key);
    return true;
  }

  void hidKeyPress(int key) {
    if (isModifier(key)) {
      storeModifier(key);
    } else {
      storeKeyPress(key);
    }
  }

  void hidKeyRelease(int key) {
    if (isModifier(key)) {
      forgetModifier(key);
    } else {
      forgetKeyPress(key);
    }
  }

  bool keyReport() {
    char cmd[64];
    int modifier = mReport.modifiers;
    int first = mReport.keys[0];
    int second = mReport.keys[1];
    int third = mReport.keys[2];
    int fourth = mReport.keys[3];
    int fifth = mReport.keys[4];
    int sixth = mReport.keys[5];
    sprintf(cmd, "AT+BLEKEYBOARDCODE=%02X-00-%02X-%02X-%02X-%02X-%02X-%02X",
            modifier, first, second, third, fourth, fifth, sixth);
    return mBle->sendCommandCheckOK(cmd);
  }

 private:
  bool isModifier(int key) {
    return key >= HID_KEY_CONTROL_LEFT && key <= HID_KEY_GUI_RIGHT;
  }

  int getModifierBit(int key) {
    if (key == HID_KEY_CONTROL_LEFT) {
      return MODIFIER_BIT_LEFT_CONTROL;
    }
    if (key == HID_KEY_SHIFT_LEFT) {
      return MODIFIER_BIT_LEFT_SHIFT;
    }
    if (key == HID_KEY_ALT_LEFT) {
      return MODIFIER_BIT_LEFT_ALT;
    }
    if (key == HID_KEY_GUI_LEFT) {
      return MODIFIER_BIT_LEFT_WINDOW;
    }
    if (key == HID_KEY_CONTROL_LEFT) {
      return MODIFIER_BIT_RIGHT_CONTROL;
    }
    if (key == HID_KEY_SHIFT_RIGHT) {
      return MODIFIER_BIT_RIGHT_SHIFT;
    }
    if (key == HID_KEY_ALT_RIGHT) {
      return MODIFIER_BIT_RIGHT_ALT;
    }
    if (key == HID_KEY_GUI_RIGHT) {
      return MODIFIER_BIT_RIGHT_WINDOW;
    }
    return 0;
  }

  void storeModifier(int key) {
    int modifier = getModifierBit(key);
    mReport.modifiers |= modifier;
  }

  void forgetModifier(int key) {
    int modifier = getModifierBit(key);
    mReport.modifiers &= ~modifier;
  }

  bool storeKeyPress(int key) {
    for (int i = 0; i < 6; i++) {
      if (mReport.keys[i] == 0) {
        mReport.keys[i] = key;
        return true;
      }
    }
    return false;
  }

  bool forgetKeyPress(int key) {
    for (int i = 0; i < 6; i++) {
      if (mReport.keys[i] == key) {
        mReport.keys[i] = 0;
        return true;
      }
    }
    return false;
  }
};

Bluetooth_ Bluetooth(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

#endif