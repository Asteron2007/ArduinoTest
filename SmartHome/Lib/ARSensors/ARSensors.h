// Library TARSensors
// Asteron Robotics Evolution
#pragma once
#include <Arduino.h>
//#define _BTN_DEB_TIME 100  // таймаут антидребезга


class TARSensor {
  public:
    TARSensor (String Name, bool Digital, String Unit, unsigned long Period, byte pin) : _pin(pin) {
      pinMode(_pin, INPUT_PULLUP);
    }

    String Name;
    String Unit;
    bool Digital;
    unsigned long Period; // polling period

    float Value;
    bool Status() //
    {
      return 0;
    }
    String ToString()
    {
      return Name + ": " + String (Value) + " " + Unit;
    }

    bool click() {
      bool btnState = digitalRead(_pin);
      if (!btnState && !_flag && millis() - _tmr >= _BTN_DEB_TIME) {
        _flag = true;
        _tmr = millis();
        return true;
      }
      if (btnState && _flag) {
        _flag = false;
        _tmr = millis();
      }
      return false;
    }


  private:
    const byte _pin;
    uint32_t _tmr;
    bool _flag;
};
