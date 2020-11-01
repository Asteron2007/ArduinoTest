// Library TARSensors
// Asteron Robotics Evolution
// http://asteron24.ru/
// asteron-2007@mail.ru
//+7 910 553-46-37


#pragma once
#include <Arduino.h>
//#define _BTN_DEB_TIME 100  // таймаут антидребезга

//
//Подключение и использование
//Чтобы соединить любой сенсор с Arduino и начать считывать его показания, нужно:
//
//Определить, какому типу сигнала соответствует сенсор: аналоговый, цифровой, сопротивление.
//Физически подключить сенсор к Arduino: по трём проводам или через breadboard, в зависимости от сенсора.
//Определить, по какому протоколу передаются данные: зависимость от напряжения, бинарный или свой собственный,
//и написать программу для их получения.
//Отдельных статьей заслуживают сенсор температуры и влажности,
//герметичный датчик температуры DS18B20 и аналоговый термометр на базе TMP36.



//Протокол датчика DHT
//Выходом датчика является цифровой сигнал. Температура и влажность передаются по одному сигнальному проводу (S).
//DHT11 общается с принимающей стороной, такой как Arduino по собственному протоколу. Коммуникация двунаправлена
//и в общих чертах выглядит так:
//
//Микроконтроллер говорит о том, что хочет считать показания. Для этого он устанавливает сигнальную
//линию в 0 на некоторое время, а затем устанавливает её в 1
//Сенсор подтверждает готовность отдать данные. Для этого он аналогично сначала устанавливает сигнальную
//линию в 0, затем в 1
//После этого сенсор передаёт последовательность 0 и 1, последовательно формирующих 5 байт (40 бит).
//В первых двух байтах передаётся температура, в третьем-четвёртом — влажность,
//в пятом — контрольная сумма, чтобы микроконтроллер смог убедиться в отсутствии ошибок считывания
//Благодаря тому, что сенсор делает измерения только по запросу, достигается энергоэффективность:
//пока общения нет, датчик потребляет ток 100 мкА.



class TARSensor
{
  public:
    TARSensor(String Name, String Placement, bool Digital, String Unit, unsigned long Period, byte Pin,
              float LowValue = 0.0F, float HiValue = 1.0F,  float MinRange = 0.0F, float MaxRange = 1.0F)
    {
      this->Name = Name;
      this->Placement = Placement;
      this->Digital = Digital;
      this->Unit = Unit;
      this->Period = Period;
      this->HiValue = HiValue;
      this->LowValue = HiValue;
      this->MinRange = MinRange;
      this->MaxRange = MaxRange;
      this->Pin = Pin;
      //pinMode(this->Pin, INPUT);
      onLow = NULL;
      onHi = NULL;
      onRead = NULL;
    }

    String Name;
    String Placement;
    String Unit;
    bool Digital;
    unsigned long Period; // polling period
    float HiValue;
    float LowValue;
    float MinRange;
    float MaxRange;
    byte Pin;
    void (*onLow)(TARSensor Sensor);
    void (*onHi)(TARSensor Sensor);
    int (*onRead)(TARSensor Sensor);

    float FloatValue;
    bool BoolValue;
    int Value;
    bool Connected() //
    {
      bool Connected = true;
      if (Connected)
        ConnectedStatus = "Connected";
      else
        ConnectedStatus = "Disconnected";
      return Connected;
    }
    String ConnectedStatus;
    String ToString()
    {
      if (Connected())
      {
        if (Digital)
          return Placement + " " + Name + ": " + String (BoolValue) + " " + Unit;
        else
          return Placement + " " + Name + ": " + String (FloatValue) + " " + Unit;
      }
      else
        return Name + ": is disconnected!" ;
    }

  private:

};

int ARAnalogSensorRead(TARSensor& Sensor)
{
  // reading period
  Sensor.Value = analogRead(Sensor.Pin);
  Sensor.FloatValue = (float)Sensor.Value * (Sensor.MaxRange - Sensor.MinRange) / 1024;
  return analogRead(Sensor.Pin);
}

int ARDigitalSensorRead(TARSensor& Sensor)
{
  // reading period
  Sensor.Value = digitalRead(Sensor.Pin);
  Sensor.BoolValue = Sensor.Value;
  return digitalRead(Sensor.Pin);
}
