// SimpleSmartHome v 1.0
// Asteron Robotics Evolution
// http://asteron24.ru/
// asteron-2007@mail.ru
//+7 910 553-46-37

#include <Arduino.h>
#include <SoftwareSerial.h>
#include  <Wire.h>
#include "lib\ARSensors\ARSensors.h"
// библиотека для работы с датчиками серии DHT
#include <TroykaDHT.h>


// Constants
const  String OwnerPhoneNumber  = "+79105544321";
const bool Debug = false;
const long BAUD = 9600;

const int SENSORSCOUNT = 4;//50;
//const int ACTUATORSCOUNT = 8;//
const String PlacementCaptions[]  = // an array of placement captions in smart home
{ "Hall", "Storage", "BedRoom", "OutDoor", "Attic", "BathRoom", "Kitchen", "OutsideGate", "HallWay" };



//  Variables
// Создать объект программного последовательного порта для связи с SIM900
// Tx и Rx SIM900 подключены к выводам 7 и 8 Arduino
SoftwareSerial mySerial(7, 8);
// создаём объект класса DHT
// передаём номер пина к которому подключён датчик и тип датчика
// типы сенсоров: DHT11, DHT21, DHT22
DHT dht(4, DHT11);
uint32_t timer = 0;

const float AlarmHiMoveDetect = 200.0F;
const float NoAlarmHiMoveDetect = 2000.0F;



TARSensor Sensors[SENSORSCOUNT] = {
  TARSensor("Knock0", 0, stAnalog, "", 100, A0, 0.0F, NoAlarmHiMoveDetect),
  TARSensor("MoveDetect0", 0, stAnalog, "", 100, A1, 0.0F, NoAlarmHiMoveDetect),
  TARSensor("Temperature0", 0, stAnalog, "C", 100, A2, 20.0F, 25.0F),
  TARSensor("KeyBoard0", 0, stAnalog, "", 100, A3, 0.0F, 1024.0F),
  //TARSensor("Sound0", 0, stAnalog, "dB", 100, A4, 0.0F, 850.0F),
  //TARSensor("Fire0", 0, stAnalog, "", 100, A5, 0.0F, 700.0F),
  //
  //TARSensor("Temperature1", 0, stTemperature, "C", 5000, 4, 19.0F, 26.0F),
  //TARSensor("Humidity0", 0, stDHTHumidity, "%", 5000, 4, 10.0F, 80.0F)
};
// 2 correct
//TARSensor Actuators[ACTUATORSCOUNT];

void InitSMSModem()
{
  // Начать последовательную связь Arduino и SIM900
  SIM900power();
  mySerial.begin(BAUD);//9600
  Serial.println(F("Initializing..."));

  while (!mySerial.available()) {           // Зацикливаем и ждем инициализацию SIM800L
    mySerial.println("AT");                  // Отправка команды AT
    delay(1000);                             // Пауза
    Serial.println(F("Connecting..."));         // Печатаем текст
  }
  mySerial.println(F("AT+CSCS=\"GSM\""));
  delay(1000);
  Serial.println(F("Connected!"));            // Печатаем текст
  mySerial.println(F("AT+CMGF=1"));           // Отправка команды AT+CMGF=1
  delay(1000);                             // Пауза
  mySerial.println(F("AT+CNMI=1,2,0,0,0"));   // Отправка команды AT+CNMI=1,2,0,0,0
  delay(1000);                             // Пауза
  mySerial.println(F("AT+CMGL=\"REC UNREAD\""));
  delay(3000);
  SendSMS(F("Ready"), OwnerPhoneNumber);
}

void setup()
{
  Serial.begin(BAUD);//9600
  dht.begin();
  while (!Serial)
  {
    // wait for serial port to connect.
  }

  InitSMSModem();

  // Sensors
  //
  //  Analog inputs                    Name Placement Digital Unit Period Pin LowValue HiValue MinRange MaxRange
  // Sensors[0] = new TARSensor("KnockSensor0", PlacementCaptions[0], false, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[1] = new TARSensor("MoveDetectSensor0", PlacementCaptions[0], false, "", 100, A1, 0.0F, 1.0F,  0.0F, 1.0F);
  // Sensors[2] = new TARSensor("TemperatureSensor0", PlacementCaptions[0], false, "", 100, A2, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[3] = new TARSensor("KeyBoard0", PlacementCaptions[0], false, "", 100, A3, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[4] = new TARSensor("SoundSensor0", PlacementCaptions[0], false, "", 100, A4, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[5] = new TARSensor("FireSensor0", PlacementCaptions[0], false, "", 100, A5, 0.0F, 1.0F,  0.0F, 1.0F);
  //
  //  Sensors[6] = new TARSensor("TemperatureSensor1", PlacementCaptions[0], false, "", 100, 4, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[7] = new TARSensor("HumiditySensor0", PlacementCaptions[0], false, "", 100, 4, 0.0F, 1.0F,  0.0F, 1.0F);

  //  // 2 connect & correct
  //  // Smoke
  //  Sensors[8] = new TARSensor("SmokeSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  // Lighting
  //  Sensors[9] = new TARSensor("LightingSensor", PlacementCaptions[1], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  // Wattering влажность почвы
  //  Sensors[10] = new TARSensor("WatteringSensor", PlacementCaptions[0], true, "", 100, 1, 0.0F, 1.0F,  0.0F, 1.0F);
  //  // Rain влажность почвы
  //  Sensors[11] = new TARSensor("RainSensor", PlacementCaptions[0], true, "", 100, 2, 0.0F, 1.0F,  0.0F, 1.0F);
  //  //  Air Quallity
  //  Sensors[12] = new TARSensor("Air Quallity", PlacementCaptions[0], true, "", 100, 3, 0.0F, 1.0F,  0.0F, 1.0F);
  //  // PhotoResistor
  //  Sensors[13] = new TARSensor("PhotoResistor", PlacementCaptions[0], true, "", 100, 4, 0.0F, 1.0F,  0.0F, 1.0F);






  //  Sensors[14] = new TARSensor("Temperature", PlacementCaptions[0], true, "", 100, 5, 0.0F, 1.0F,  0.0F, 1.0F);
  //
  //  // Fire
  //  Sensors[15] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[16] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[17] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[18] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[19] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  // Smoke
  //  Sensors[20] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[21] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[22] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[23] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[24] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  // Move detect in home 10 pts
  //  Sensors[25] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[26] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[27] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[28] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[29] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[30] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[31] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[32] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[33] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  //  10 reserved
  //  Sensors[34] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  // Move detect OutDoors 11 pts
  //  Sensors[35] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[36] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[37] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[38] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[39] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[40] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[41] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[42] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[43] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[44] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //
  //  // Lighting
  //  Sensors[45] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[46] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //
  //  // Wattering
  //  Sensors[47] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //  Sensors[48] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
  //
  //  // OutDoors temperature
  //  Sensors[49] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);


  // Actuators
  //  for (int i = 0; i < ACTUATORSCOUNT; i++)
  //    Actuators[i] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);


  Sensors[0].Read = ARAnalogSensorRead;
  Sensors[1].Read = ARAnalogSensorRead;
  Sensors[2].Read = ARAnalogTemperatureSensorRead;
  Sensors[3].Read = ARAnalogSensorRead;
//  Sensors[4].Read = ARAnalogSensorRead;
//  Sensors[5].Read = ARAnalogSensorRead;
//  Sensors[6].Read = getTemperature;
//  Sensors[7].Read = getHumidity;

  Sensors[0].onHi = Alarm;
  Sensors[1].onHi = Alarm;
  Sensors[2].onHi = Alarm;
  Sensors[3].onHi = Alarm;
//  Sensors[4].onHi = Alarm;
//  Sensors[5].onHi = Alarm;
//  Sensors[6].onHi = Alarm;
//  Sensors[7].onHi = Alarm;

  //Sensors[0].onLow = Alarm;
  //Sensors[1].onLow = Alarm;
  //Sensors[2].onLow = Alarm;
  //Sensors[3].onLow = Alarm;
  //Sensors[4].onLow = Alarm;
  //Sensors[5].onLow = Alarm;
  //Sensors[6].onLow = Alarm;
  //Sensors[7].onLow = Alarm;





}
bool Alarmed = false;
void loop()
{

  for (int i = 0; i < SENSORSCOUNT; i++)
  {
    if (Sensors[i].Read)
    {
      Sensors[i].Read(Sensors[i]);
      if (Debug)
        Serial.println(String(i) + ". " + PlacementCaptions[Sensors[i].Placement] + F(": Sensor") +
                       Sensors[i].Name + ": " + Sensors[i].Value + Sensors[i].Unit);
    }

    delay(100);
    Sensors[i].CheckHi();
    delay(100);
    Sensors[i].CheckLow();
    delay(100);
  }

  String inputString;
  String Message = "";


//  if (!mySerial.available())
//  {
//    mySerial.println("AT");
//    delay(1000);
//    if (!mySerial.available())
//    {
//      Serial.println(F("RESET"));
//      SIM900Restart();
//    }
//  }


  if (mySerial.available())// Проверяем, если есть доступные данные
  {
    delay(10);                                 // Пауза

    while (mySerial.available()) {              // Проверяем, есть ли еще данные.
      inputString += (char)mySerial.read();                // Записываем считанный байт в массив inputString
      delay(10);
    }

    //while ((inputString.indexOf(F("+CIEV:")) > -1) || (inputString.indexOf(F("+CMT:")) > -1));
    delay(100);                               // Пауза
    if (Debug)
      Serial.println(inputString);
    // Отправка в "Мониторинг порта" считанные данные
    inputString.toUpperCase();                  // Меняем все буквы на заглавные

    if ((inputString.indexOf(F("+CIEV:")) > -1) && (inputString.indexOf(F("+CMT:")) > -1)
        && (inputString.indexOf(OwnerPhoneNumber) > -1))
    {
      if (inputString.indexOf(F("DATA")) > -1) {     // Проверяем полученные данные
        for (int i = 0; i < SENSORSCOUNT; i++)
        {
          if (Sensors[i].Read)
            Sensors[i].Read(Sensors[i]);
          Message = String(i) + ". " + PlacementCaptions[Sensors[i].Placement] + F(": Sensor")
                    + Sensors[i].Name + ": " + Sensors[i].Value + Sensors[i].Unit;
          Serial.println(Message);
          SendSMS(Message, OwnerPhoneNumber); // send SMS
          delay(1000);
        }

      }
      //постановка на охрну
      if (inputString.indexOf(F("ALARM")) > -1) {     // Проверяем полученные данные
        Alarmed = true;
        Sensors[0].HiValue = AlarmHiMoveDetect;
        Sensors[1].HiValue = AlarmHiMoveDetect;
        Message = F("ALARMED");
        Serial.println(Message);
        SendSMS(Message, OwnerPhoneNumber); // send SMS
        delay(1000);
      }
      if (inputString.indexOf(F("FREE")) > -1) {     // Проверяем полученные данные
        Alarmed = false;
        Sensors[0].HiValue = NoAlarmHiMoveDetect;
        Sensors[1].HiValue = NoAlarmHiMoveDetect;
        Message = F("DISALARMED");
        Serial.println(Message);
        SendSMS(Message, OwnerPhoneNumber); // send SMS
        delay(1000);
      }

    }

    delay(50);
    if (inputString.indexOf(F("OK")) == -1) {
      mySerial.println(F("AT+CMGD=2,4"));
      delay(1000);
    }
    inputString = "";
  }





}





// functions
void SIM900power()
{
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  delay(1000);
  digitalWrite(9, HIGH);
  delay(2000);
  digitalWrite(9, LOW);
  delay(3000);
}
//2 correct
void SIM900Restart()
{
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  delay(1000);
  digitalWrite(10, HIGH);
  delay(2000);
  digitalWrite(10, LOW);
  delay(3000);
}



void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    // Пересылка того, что было получено с аппаратного последовательного порта,
    // на программный последовательный порт
    mySerial.write(Serial.read());
  }
  while (mySerial.available())
  {
    // Пересылка того, что было получено с программного последовательного порта,
    // на аппаратный последовательный порт
    Serial.write(mySerial.read());
  }
}

void SendSMS(String text, String phone)  // Процедура Отправка SMS
{
  //Serial.println(F("SMS send started"));
  mySerial.println("AT+CMGS=\"" + phone + "\"");
  delay(500);
  mySerial.print(text);
  delay(500);
  mySerial.print((char)26);
  delay(500);
  Serial.println(F("SMS send complete"));
  delay(500);
}

//void SendSMS(String text, String phone)  // Процедура Отправка SMS
//{
//  //Serial.println(F("SMS send started"));
//  Serial.println("AT+CMGS=\"" + phone + "\"");
//  delay(500);
//  Serial.print(text);
//  delay(500);
//  Serial.println(F("SMS send complete"));
//  delay(500);
//}




void Alarm(TARSensor& Sensor)
{
  SendSMS(String(PlacementCaptions[Sensor.Placement] + F(": Sensor") + Sensor.Name + ": " +
                 Sensor.Value + Sensor.Unit), OwnerPhoneNumber);
  Serial.println("ALARM: " + PlacementCaptions[Sensor.Placement] + F(": Sensor") + Sensor.Name + ": " +
                 Sensor.Value + Sensor.Unit);
}



//
///*
//Тестировалось на Arduino IDE 1.8.5
//Дата тестирования 21.06.2019г.
//*/
//
//#include <SoftwareSerial.h>
//#include  <Wire.h>                                         // Подключаем библиотеку Wire
//#include  <Adafruit_Sensor.h>                             // Подключаем библиотеку Adafruit_Sensor
//#include  <Adafruit_BME280.h>                             // Подключаем библиотеку Adafruit_BME280
//
//SoftwareSerial mySerial(3, 2);                           // Выводы SIM800L Tx & Rx подключены к выводам Arduino 3 и 2
//#define SEALEVELPRESSURE_HPA (1013.25)                   // Задаем высоту
//Adafruit_BME280 bme;
//
//char incomingByte;
//String inputString;
//
//float t;
//float p;
//float h;
//
//void setup()
//{
//  Serial.begin(9600);
//  mySerial.begin(9600);
//
//  bme.begin(0x76);
//
//
//  while(!mySerial.available()){             // Зацикливаем и ждем инициализацию SIM800L
//   mySerial.println("AT");                  // Отправка команды AT
//   delay(1000);                             // Пауза
//   Serial.println("Connecting...");         // Печатаем текст
//   }
//   Serial.println("Connected!");            // Печатаем текст
//   mySerial.println("AT+CMGF=1");           // Отправка команды AT+CMGF=1
//   delay(1000);                             // Пауза
//   mySerial.println("AT+CNMI=1,2,0,0,0");   // Отправка команды AT+CNMI=1,2,0,0,0
//   delay(1000);                             // Пауза
//   mySerial.println("AT+CMGL=\"REC UNREAD\"");
//}
//
//void loop()
//
//{
//  if(mySerial.available()){                       // Проверяем, если есть доступные данные
//      delay(100);                                 // Пауза
//
//      while(mySerial.available()){                // Проверяем, есть ли еще данные.
//      incomingByte = mySerial.read();             // Считываем байт и записываем в переменную incomingByte
//      inputString += incomingByte;                // Записываем считанный байт в массив inputString
//      }
//
//      delay(10);                                  // Пауза
//      Serial.println(inputString);                // Отправка в "Мониторинг порта" считанные данные
//      inputString.toUpperCase();                  // Меняем все буквы на заглавные
//
//      if (inputString.indexOf(AdminPhone) > -1)   {

//if (inputString.indexOf("+CMT") > -1)     {
//  if (inputString.indexOf("DATA") > -1) {     // Проверяем полученные данные

//        t = bme.readTemperature();
//        h = bme.readHumidity();
//        p = (bme.readPressure() / 100.0F);
//        sms(String("Temperature: " + String(t) + " *C " + " Humidity: " + String(h) + " % " + " Pressure: " + String(p) + " hPa"), String("+7xxxxxxxxxx")); // Отправка SMS
//        }
//
//        delay(50);
//      if (inputString.indexOf("OK") == -1){
//          mySerial.println("AT+CMGDA=\"DEL ALL\"");
//          delay(1000);}
//
//          inputString = "";}}
//
//void sms(String text, String phone)  // Процедура Отправка SMS
//{
//  Serial.println("SMS send started");
//  mySerial.println("AT+CMGS=\"" + phone + "\"");
//  delay(500);
//  mySerial.print(text);
//  delay(500);
//  mySerial.print((char)26);
//  delay(500);
//  Serial.println("SMS send complete");
//  delay(2000);
//}


String getTemperature(TARSensor& Sensor)
{
  //int Period = 1000 * 2;
  delay(Sensor.Period);
  //if (millis() - timer >= Period) {
  // ваше действие

  // считывание данных с датчика
  dht.read();
  // проверяем состояние данных
  switch (dht.getState()) {
    // всё OK
    case DHT_OK:
      // выводим показания влажности и температуры
      return String(dht.getTemperatureC());
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
  delay(Sensor.Period);
}

String getHumidity(TARSensor& Sensor)
{

  //int Period = 1000 * 2;

  delay(Sensor.Period);
  //if (millis() - timer >= Period) {
  // ваше действие

  // считывание данных с датчика
  dht.read();
  // проверяем состояние данных
  switch (dht.getState()) {
    // всё OK
    case DHT_OK:
      // выводим показания влажности и температуры
      return String(dht.getHumidity());
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
  delay(Sensor.Period);
}
