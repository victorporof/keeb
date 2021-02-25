#ifndef Keyboard_h
#define Keyboard_h

#include "Switch.h"

class Keys {
  int* mRowPins;
  int* mColPins;
  int mRowCount, mColCount;
  Switch* mKeys;

 public:
  static const int kInvalid = -1;

  Keys(int* rowPins, int rowCount, int* colPins, int colCount)
      : mRowPins(rowPins),
        mColPins(colPins),
        mRowCount(rowCount),
        mColCount(colCount) {
    mKeys = new Switch[rowCount * colCount];
  }

  ~Keys() { delete[] mKeys; }

  void begin() {
    for (int row = 0; row < mRowCount; row++) {
      pinMode(mRowPins[row], INPUT);
    }
    for (int col = 0; col < mColCount; col++) {
      pinMode(mColPins[col], INPUT_PULLUP);
    }
  }

  void update() { readMatrix(); }

  int getKeyCount() { return mRowCount * mColCount; }

  int getKeyIndex(int row, int col) { return row * mColCount + col; }

  int getKeyState(int i) {
    if (i < 0 || i >= getKeyCount()) {
      return kInvalid;
    }
    return mKeys[i].getState();
  }

 private:
  void readMatrix() {
    for (int col = 0; col < mColCount; col++) {
      int colPin = mColPins[col];
      pinMode(colPin, OUTPUT);
      digitalWrite(colPin, LOW);

      for (int row = 0; row < mRowCount; row++) {
        int rowPin = mRowPins[row];
        pinMode(rowPin, INPUT_PULLUP);

        int reading = digitalRead(rowPin);
        mKeys[getKeyIndex(row, col)].setReading(reading);

        pinMode(rowPin, INPUT);
      }

      pinMode(colPin, INPUT);
    }
  }
};

#endif