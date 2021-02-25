#ifndef Device_h
#define Device_h

#include <Keyboard.h>
#include <Mouse.h>

#define BLUETOOTH_DEVICE_NAME "Awesome Keyboard"

#include "Bluetooth.h"

class Device_ {
  bool mUsbEnabled{};
  bool mBleEnabled{};

 public:
  void begin() {
    Keyboard.begin();
    Mouse.begin();
    Bluetooth.begin();
  }

  void useUsbOnly() {
    mUsbEnabled = true;
    mBleEnabled = false;
  }

  void useBleOnly() {
    mBleEnabled = true;
    mUsbEnabled = false;
  }

  void keyPress(char ch) {
    if (mUsbEnabled) {
      Keyboard.press(ch);
    }
    if (mBleEnabled) {
      Bluetooth.keyPress(ch);
    }
  }

  void keyRelease(char ch) {
    if (mUsbEnabled) {
      Keyboard.release(ch);
    }
    if (mBleEnabled) {
      Bluetooth.keyRelease(ch);
    }
  }

  void keyReport() {
    if (mBleEnabled) {
      Bluetooth.keyReport();
    }
  }

  void mousePress(int button) {
    if (mUsbEnabled) {
      Mouse.press(button);
    }
    if (mBleEnabled) {
      Bluetooth.mousePress(button);
    }
  }

  void mouseRelease(int button) {
    if (mUsbEnabled) {
      Mouse.release(button);
    }
    if (mBleEnabled) {
      Bluetooth.mouseRelease();
    }
  }

  void mouseMove(int x, int y, int scroll) {
    if (mUsbEnabled) {
      Mouse.move(x, y, scroll);
    }
    if (mBleEnabled) {
      Bluetooth.mouseMove(x, y, scroll);
    }
  }
};

Device_ Device;

#endif