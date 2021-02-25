#ifndef Joystick_h
#define Joystick_h

#include "Switch.h"

class Joystick {
  const int kRange = 12;
  const int kThreshold = 1;
  const int kCenter = kRange / 2;
  const float kAcceleration = 1.4;

  int mXPin, mYPin;
  int mInvertX, mInvertY;
  int mX = 0;
  int mY = 0;

 public:
  Joystick(int xPin, int yPin, bool invertX = true, bool invertY = true)
      : mXPin(xPin), mYPin(yPin), mInvertX(invertX), mInvertY(invertY) {}

  void begin() {
    // noop
  }

  void update() {
    mX = readAxis(mXPin, mInvertX);
    mY = readAxis(mYPin, mInvertY);
  }

  bool hasX() { return mX != 0; }

  bool hasY() { return mY != 0; }

  bool hasInput() { return hasX() || hasY(); }

  int getX() { return mX; }

  int getY() { return mY; }

 private:
  int readAxis(int axis, bool invert) {
    int reading = analogRead(axis);
    reading = map(reading, 0, 1023, 0, kRange);

    int distance = invert ? reading - kCenter : kCenter - reading;
    if (abs(distance) <= kThreshold) {
      return 0;
    }

    if (distance > 0) {
      distance = (int)pow(kAcceleration, distance);
    } else if (distance < 0) {
      distance = -(int)pow(kAcceleration, -distance);
    }

    return distance;
  }
};

class JoystickWithButton : public Joystick {
  int mButtonPin;
  Switch mButton{};

 public:
  JoystickWithButton(int buttonPin, int xPin, int yPin, bool invertX = true,
                     bool invertY = true)
      : Joystick(xPin, yPin, invertX, invertY), mButtonPin(buttonPin) {}

  void begin() {
    Joystick::begin();
    pinMode(mButtonPin, INPUT_PULLUP);
  }

  void update() {
    Joystick::update();
    readButton();
  }

  int getButtonState() { return mButton.getState(); }

 private:
  void readButton() {
    int reading = digitalRead(mButtonPin);
    mButton.setReading(reading);
  }
};

#endif