// Library TARActuators
// Asteron Robotics Evolution
// http://asteron24.ru/
// asteron-2007@mail.ru
//+7 910 553-46-37


#pragma once
#include <Arduino.h>

enum TARActuatorType  {
  atDigital,
  atAnalog,
  
};

class TARActuator
{
  public:
    TARActuator(char* Name, byte Placement, TARActuatorType ActuatorType, unsigned long Period,
                byte Pin, float LowValue = 0.0F, float HiValue = 1.0F, bool Active = false)
    {
      this->Name = Name;
      this->Placement = Placement;
      this->ActuatorType = ActuatorType;
      this->Period = Period;
      this->HiValue = HiValue;
      this->LowValue = LowValue;
      this-> Active = Active;
      this->Pin = Pin;
      pinMode(this->Pin, OUTPUT);
      onRun = NULL;
    }

    String Name;
    byte Placement;
    TARActuatorType ActuatorType;
    unsigned long Period; // polling period
    float HiValue;
    float LowValue;
    bool Active;
    byte Pin;
    String Value;
    void (*onRun)(TARActuator& Actuator);
    void Run()
    {
      if (Active)
        if (onRun) {
          onRun(*this);
        }
    }



  private:

};


void ARAnalogActuatorRun(TARActuator& Actuator)
{
  digitalWrite(Actuator.Pin, Actuator.Value.toInt());
  delay(Actuator.Period);
}

void ARDigitalActuatorRun(TARActuator& Actuator)
{
  digitalWrite(Actuator.Pin, Actuator.Value.toInt());
  delay(Actuator.Period);
}
