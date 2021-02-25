#ifndef Switch_h
#define Switch_h

class Switch {
  int mState = kJustReleased;

 public:
  static const int kJustReleased = 0;
  static const int kJustPressed = 1;
  static const int kStillReleased = 2;
  static const int kStillPressed = 3;

  int getState() {
    int state = mState;
    if (mState == kJustReleased) {
      mState = kStillReleased;
    } else if (mState == kJustPressed) {
      mState = kStillPressed;
    }
    return state;
  }

  bool setReading(int reading, int pressed = LOW) {
    if (reading == pressed) {
      return press();
    }
    return release();
  }

  bool press() {
    if (mState == kJustReleased || mState == kStillReleased) {
      mState = kJustPressed;
      return true;
    }
    return false;
  }

  bool release() {
    if (mState == kJustPressed || mState == kStillPressed) {
      mState = kJustReleased;
      return true;
    }
    return false;
  }
};

#endif