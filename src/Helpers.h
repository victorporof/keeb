#ifndef Helpers_h
#define Helpers_h

void error(const __FlashStringHelper* err) {
  Serial.println(err);
  while (1)
    ;
}

#endif