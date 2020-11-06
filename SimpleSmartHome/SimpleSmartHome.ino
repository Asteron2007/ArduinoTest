// SimpleSmartHome v 1.0
// Asteron Robotics Evolution
// http://asteron24.ru/
// asteron-2007@mail.ru
//+7 910 553-46-37

//2 do
// - gprs
// - Alarm
// - Sensors
// - log (sd card)
// - comunication
// - Macros DEBUG
// - NN
// - 

#include <Arduino.h>
#include <SoftwareSerial.h>
#include  <Wire.h>
#include "lib\ARSensors\ARSensors.h"

// Constants
const  String OwnerPhoneNumber  = "+79105544321";
const bool Debug = false;
const long BAUD = 9600;

const int SENSORSCOUNT = 9;//50;
//const int ACTUATORSCOUNT = 8;//
const String PlacementCaptions[]  = // an array of placement captions in smart home
{ "Hall", "Storage", "BedRoom", "OutDoor", "Attic", "BathRoom", "Kitchen", "OutsideGate", "HallWay" };

//const float AlarmHiMoveDetect = 200.0F;
//const float NoAlarmHiMoveDetect = 2000.0F;

//  Variables
// Создать объект программного последовательного порта для связи с SIM900
// Tx и Rx SIM900 подключены к выводам 7 и 8 Arduino
SoftwareSerial mySerial(7, 8);

uint32_t timer = 0;



bool Armed = false;

//bool KnockSensorAlarm = false;
//bool MoveDetectAlarm = false;
//bool LowTemperatureAlarm = false;
//bool HiTemperatureAlarm = false;
//bool SoundSensorAlarm = false;
//bool FireSensorAlarm = false;
//bool LowHumidityAlarm = false;
//bool HiHumidityAlarm = false;






// Sensors
//
TARSensor Sensors[SENSORSCOUNT] = {
  //  Analog inputs
  // Name Placement Digital Unit Period Pin LowValue HiValue
  TARSensor("Knock0", 0, stAnalog, "", 0, A0, -1.0F, 200.0F, Armed),
  TARSensor("Move0", 0, stAnalog, "", 00, A1, -1.0F, 200.0F, Armed),
  TARSensor("Temp0", 0, stAnalog, "C", 00, A2, 18.0F, 30.0F, true),//25
  TARSensor("---", 0, stAnalog, "", 00, A3, 0.0F, 1024.0F, true),
  TARSensor("Sound0", 0, stAnalog, "dB", 00, A4, 0.0F, 850.0F, true),
  TARSensor("Fire0", 0, stAnalog, "", 00, A5, 0.0F, 700.0F, true),
  //  Digital inputs
  TARSensor("Temp", 0, stTemperature, "C", 500, 4, 15.0F, 32.0F, true),
  TARSensor("Humidity0", 0, stDHTHumidity, "%", 500, 4, 40.0F, 70.0F, true),
  TARSensor("Ligting0", 0, stDigital, "", 0, 2, 0.0F, 1.0F, true),



  //  // 2 connect & correct
  //  // Smoke
  // GerconSensor
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
  //  // OutDoors temperature
  //  Sensors[49] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);

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

  while (!Serial)
  {
    // wait for serial port to connect.
  }

  InitSMSModem();

  // Actuators
  //  for (int i = 0; i < ACTUATORSCOUNT; i++)
  //    Actuators[i] = new TARSensor("GerconSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);




  Sensors[0].onRead = ARAnalogSensorRead;
  Sensors[1].onRead = ARAnalogSensorRead;
  Sensors[2].onRead = ARAnalogTemperatureSensorRead;
  Sensors[3].onRead = ARAnalogSensorRead;
  Sensors[4].onRead = ARAnalogSensorRead;
  Sensors[5].onRead = ARAnalogSensorRead;
  Sensors[6].onRead = DHTTemperatureRead;
  Sensors[7].onRead = DHTHumidityRead;
  Sensors[8].onRead = ARDigitalSensorRead;

  Sensors[0].onHi = Alarm;
  Sensors[1].onHi = Alarm;
  Sensors[2].onHi = Alarm;
  Sensors[3].onHi = Alarm;
  Sensors[4].onHi = Alarm;
  Sensors[5].onHi = Alarm;
  Sensors[6].onHi = Alarm;
  Sensors[7].onHi = Alarm;
  Sensors[8].onHi = Alarm;

  //Sensors[0].onLow = Alarm;
  //Sensors[1].onLow = Alarm;
  Sensors[2].onLow = Alarm;
  //Sensors[3].onLow = Alarm;
  //Sensors[4].onLow = Alarm;
  //Sensors[5].onLow = Alarm;
  Sensors[6].onLow = Alarm;
  //Sensors[7].onLow = Alarm;

}

void loop()
{
  //Serial.println(getTemperature(Sensors[0]));
  //Serial.println(getHumidity(Sensors[1]));
  //Serial.println("-");

  for (int i = 0; i < SENSORSCOUNT; i++)
  {
    Sensors[i].Read();

    if (Debug)
      Serial.println(String(i) + ". " + PlacementCaptions[Sensors[i].Placement] + F(": Sensor") +
                     Sensors[i].Name + ": " + Sensors[i].Value + Sensors[i].Unit);
    delay(1000);
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
        if (Armed)
          Message = F("ARMED");
        else
          Message = F("DISARMED");
        SendSMS(Message, OwnerPhoneNumber); // send SMS

        for (int i = 0; i < SENSORSCOUNT; i++)
        {
          Sensors[i].Read();
          Message = String(i) + ". " + PlacementCaptions[Sensors[i].Placement] + F(": Sensor")
                    + Sensors[i].Name + ": " + Sensors[i].Value + Sensors[i].Unit;
          Serial.println(Message);
          SendSMS(Message, OwnerPhoneNumber); // send SMS
          delay(1000);
        }

      }
      //постановка на охрну
      if (inputString.indexOf(F("ARM")) > -1) {     // Проверяем полученные данные
        Armed = true;
        //Sensors[0].HiValue = AlarmHiMoveDetect;
        //Sensors[1].HiValue = AlarmHiMoveDetect;


        for (int i = 0; i < SENSORSCOUNT; i++)
          Sensors[i].Armed = true;

        Message = F("ARMED");
        Serial.println(Message);
        SendSMS(Message, OwnerPhoneNumber); // send SMS
        delay(1000);
      }
      // снятие с охраны
      if (inputString.indexOf(F("FREE")) > -1) {     // Проверяем полученные данные
        Armed = false;
        //Sensors[0].HiValue = NoAlarmHiMoveDetect;
        //Sensors[1].HiValue = NoAlarmHiMoveDetect;
        Message = F("DISARMED");
        Serial.println(Message);
        SendSMS(Message, OwnerPhoneNumber); // send SMS
        delay(1000);
      }
      // снятие с охраны
      if (inputString.indexOf(F("HELP")) > -1) {     // Проверяем полученные данные

        Message = F("ARM\nFREE\nDATA\nRESET\nHELP\nSTATUS\n");
        Serial.println(Message);
        SendSMS(Message, OwnerPhoneNumber); // send SMS
        delay(1000);
      }
      // STATUS
      if (inputString.indexOf(F("STATUS")) > -1) {     // Проверяем полученные данные

        if (Armed)
          Message = F("ARMED");
        else
          Message = F("DISARMED");
        SendSMS(Message, OwnerPhoneNumber); // send SMS

        for (int i = 0; i < SENSORSCOUNT; i++)
        {
          Sensors[i].Read();
          Message = String(i) + ". " + PlacementCaptions[Sensors[i].Placement] + F(": Sensor")
                    + Sensors[i].Name + ": " + Sensors[i].Value + Sensors[i].Unit ;
          if (Sensors[i].LowAlarmed)
            Message += F(" LowAlarm");
          if (Sensors[i].HiAlarmed)
            Message += F(" HiAlarm");
          if (Sensors[i].Armed)
            Message += F(" IsArmed");
          else Message += F(" IsDisArmed");
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
//
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




void Alarm(TARSensor & Sensor)
{
  SendSMS(String(PlacementCaptions[Sensor.Placement] + F(": Sensor") + Sensor.Name + ": " +
                 Sensor.Value + Sensor.Unit  + "\n"), OwnerPhoneNumber);
  Serial.println("ALARM: " + PlacementCaptions[Sensor.Placement] + F(": Sensor") + Sensor.Name + ": " +
                 Sensor.Value + Sensor.Unit + "\n");
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
