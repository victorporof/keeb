#include "src/Device.h"
#include "src/Joystick.h"
#include "src/Keys.h"

int rowPins[] = {13, 12, 11, 10};
int colPins[] = {9, 6, 5};
const int rowCount = sizeof(rowPins) / sizeof(rowPins[0]);
const int colCount = sizeof(colPins) / sizeof(colPins[0]);

char charmap[rowCount][colCount] = {
    {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'*', '0', '#'}};

Keys keypad(rowPins, rowCount, colPins, colCount);
JoystickWithButton leftThumb(3, A0, A1);
JoystickWithButton rightThumb(2, A2, A3);

void setup() {
  Device.begin();
  Device.useBleOnly();

  keypad.begin();
  leftThumb.begin();
  rightThumb.begin();
}

void loop() {
  keypad.update();
  leftThumb.update();
  rightThumb.update();

  bool hasKeyUpdate = false;

  for (int row = 0; row < rowCount; row++) {
    for (int col = 0; col < colCount; col++) {
      int state = keypad.getKeyState(keypad.getKeyIndex(row, col));
      char ch = charmap[row][col];
      if (state == Switch::kJustPressed) {
        Device.keyPress(ch);
        hasKeyUpdate = true;
      } else if (state == Switch::kJustReleased) {
        Device.keyRelease(ch);
        hasKeyUpdate = true;
      }
    }
  }

  if (hasKeyUpdate) {
    Device.keyReport();
  }

  int leftButtonState = leftThumb.getButtonState();
  if (leftButtonState == Switch::kJustPressed) {
    Device.mousePress(MOUSE_RIGHT);
  } else if (leftButtonState == Switch::kJustReleased) {
    Device.mouseRelease(MOUSE_RIGHT);
  }

  int rightButtonState = rightThumb.getButtonState();
  if (rightButtonState == Switch::kJustPressed) {
    Device.mousePress(MOUSE_LEFT);
  } else if (rightButtonState == Switch::kJustReleased) {
    Device.mouseRelease(MOUSE_LEFT);
  }

  if (leftThumb.hasInput() || rightThumb.hasInput()) {
    Device.mouseMove(rightThumb.getX(), rightThumb.getY(), leftThumb.getY());
  }

  delay(5);
}
