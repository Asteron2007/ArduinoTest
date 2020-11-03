// Library TARSensors
// Asteron Robotics Evolution
// http://asteron24.ru/
// asteron-2007@mail.ru
//+7 910 553-46-37


#pragma once
#include <Arduino.h>
//#define _BTN_DEB_TIME 100  // таймаут антидребезга
// библиотека для работы с датчиками серии DHT
#include <TroykaDHT.h>
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

enum TARSensorType  {
  stDigital,
  stAnalog,
  stDHTHumidity,
  stTemperature,
};

class TARSensor
{
  public:
    TARSensor(char* Name, byte Placement, TARSensorType SensorType, char* Unit, unsigned long Period,
              byte Pin, float LowValue = 0.0F, float HiValue = 1.0F, bool Armed = false)
    {
      this->Name = Name;
      this->Placement = Placement;
      this->SensorType = SensorType;
      this->Unit = Unit;
      this->Period = Period;
      this->HiValue = HiValue;
      this->LowValue = LowValue;
      this->LowAlarmed = false;
      this->HiAlarmed = false;
      this-> Armed = Armed;
      this->Pin = Pin;
      pinMode(this->Pin, INPUT);
      onLow = NULL;
      onHi = NULL;
      onRead = NULL;
    }

    String Name;
    byte Placement;
    String Unit;
    TARSensorType SensorType;
    unsigned long Period; // polling period
    float HiValue;
    float LowValue;
    bool LowAlarmed;
    bool HiAlarmed;
    bool Armed;
    byte Pin;
    void (*onLow)(TARSensor& Sensor);
    void (*onHi)(TARSensor& Sensor);
    //String (*onRead)(TARSensor& Sensor);
    void (*onRead)(TARSensor& Sensor);
    void Read()
    {
      if (onRead) {
        onRead(*this);
        CheckHi();
        CheckLow();
      }
    }
    void CheckHi()
    {
      if (Armed)
        if (!HiAlarmed)
          if (HiValue <= Value.toFloat())
          {
            HiAlarmed = true;
            if (onHi)
              onHi(*this);
          }
          else HiAlarmed = false;
    }
    void CheckLow()
    {
      if (Armed)
        if (!LowAlarmed)
          if (LowValue >= Value.toFloat())
          {            
            LowAlarmed = true;
            if (onLow)
              onLow(*this);
          }
          else LowAlarmed = false;
    }

    String Value;
    //    bool Connected() //
    //    {
    //      bool Connected = true;
    //      if (Connected)
    //        ConnectedStatus = F("Connected");
    //      else
    //        ConnectedStatus = F("Disconnected");
    //      return Connected;
    //    }
    //    String ConnectedStatus;
    //    String ToString()
    //    {
    //      return "";
    //    }

  private:

};
// функция считывает аналоговый вход заданное количество раз
// и возвращает отфильтрованное значение
int readMean(int pin, int samples) {
  // переменная для хранения суммы считанных значений
  int sum = 0;
  // чтение и складывание значений
  for (int i = 0; i < samples; i++) {
    sum = sum + analogRead(pin);
  }
  // делим сумму значений на количество измерений
  sum = sum / samples;
  // возвращаем среднее значение
  return sum;
}



void ARAnalogSensorRead(TARSensor& Sensor)
{
  Sensor.Value = String(analogRead(Sensor.Pin));
  delay(Sensor.Period);
  return ;//Sensor.Value;
}

void ARDigitalSensorRead(TARSensor& Sensor)
{
  Sensor.Value = String(digitalRead(Sensor.Pin));
  delay(Sensor.Period);
  return ;//Sensor.Value;
}



void ARAnalogTemperatureSensorRead(TARSensor& Sensor)
{ //ky-013
  double Temp;
  Temp = log(((10240000 / readMean(Sensor.Pin, 20)) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp ); // Уравнение Стейнхарта-Харта
  Temp = Temp - 273.15; // Кельвин -> Цельсий

  Sensor.Value = String(Temp);
  //Sensor.Value = String((readMean(Sensor.Pin, 30) * 5.0 / 1024 + 0.5) * 100 - 273.15);
  delay(Sensor.Period);
  return ;//Sensor.Value;
}

void DHTTemperatureRead(TARSensor& Sensor)
{
  // создаём объект класса DHT
  // передаём номер пина к которому подключён датчик и тип датчика
  // типы сенсоров: DHT11, DHT21, DHT22
  DHT dht(4, DHT11);
  dht.begin();
  //int Period = 1000 * 2;
  //delay(Sensor.Period);
  //if (millis() - timer >= Period) {
  // ваше действие

  // считывание данных с датчика
  dht.read();
  // проверяем состояние данных
  switch (dht.getState()) {
    // всё OK
    case DHT_OK:
      // выводим показания влажности и температуры
      Sensor.Value = String(dht.getTemperatureC());
      delay(Sensor.Period);
      return ;//Sensor.Value;
      break;
    // ошибка контрольной суммы
    case DHT_ERROR_CHECKSUM:
      Serial.println("Checksum error");
      break;
    // превышение времени ожидания
    case DHT_ERROR_TIMEOUT:
      Serial.println("Time out error");
      break;
    // данных нет, датчик не реагирует или отсутствует
    case DHT_ERROR_NO_REPLY:
      Serial.println("Sensor not connected");
      break;
  }
  //  timer += Period;
  //}
}

//String DHTHumidityRead(TARSensor& Sensor)
void DHTHumidityRead(TARSensor& Sensor)
{
  // создаём объект класса DHT
  // передаём номер пина к которому подключён датчик и тип датчика
  // типы сенсоров: DHT11, DHT21, DHT22
  DHT dht(4, DHT11);
  dht.begin();
  //int Period = 1000 * 2;

  //delay(Sensor.Period);
  //  if (millis() - timer >= Period) {
  //   ваше действие

  // считывание данных с датчика
  dht.read();
  // проверяем состояние данных
  switch (dht.getState()) {
    // всё OK
    case DHT_OK:
      // выводим показания влажности и температуры
      delay(500);
      Sensor.Value = String(dht.getHumidity());
      delay(Sensor.Period);
      //return Sensor.Value;
      return;
      break;
    // ошибка контрольной суммы
    case DHT_ERROR_CHECKSUM:
      Serial.println(F("Checksum error"));
      break;
    // превышение времени ожидания
    case DHT_ERROR_TIMEOUT:
      Serial.println(F("Time out error"));
      break;
    // данных нет, датчик не реагирует или отсутствует
    case DHT_ERROR_NO_REPLY:
      Serial.println(F("Sensor not connected"));
      break;
  }
  //timer += Period;
  //}
}
