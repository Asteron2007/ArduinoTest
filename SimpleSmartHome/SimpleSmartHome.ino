
// SimpleSmartHome v 1.0
// Asteron Robotics Evolution
// http://asteron24.ru/
// asteron-2007@mail.ru
//+7 910 553-46-37

// based on Arduino MEGA


//2 DO
// - RING
// - gprs
//   - Email
//   - on server AG
// - Alarm
//   - time
//   - Relay
//   - servo
//   - beeper
// - Sensors
// - logs (sd card)
// - comunication
// - Macros by ProcessorType
// - Actuators
//   - hit light
//   - humidity
//   - Close Valve
//   - Heat/ vent
//   - beeper
// - Video
// - Shedule
// - clean code - Release
// - optimize
// - AI onboard -> evolution
// timer on Alarm
// reset Alarms
// Android application (SmartHome)
// Arduino Mega
// Arduino Duo
// - NN
// - Reboot

#include <Arduino.h>
#include <SoftwareSerial.h>
#include  <Wire.h>
#include "lib\ARSensors\ARSensors.h"
#include "lib\ARActuators\ARActuators.h"

#define DEBUG = true;// true - debug version/ false - release version
//#define PLATFORM_UNO;// true - debug version/ false - release version
#define PLATFORM_MEGA = true;// true - debug version/ false - release version

#define PERIOD 1000*60*60 // 1 hour
uint32_t timer = 0;


// Constants
const char* OwnerPhoneNumber  = "+79105544321";
const long BAUD = 9600;



const byte CommandCount = 7;
const char* CommandsList[CommandCount]  = // an array list of commads for smart home
{ "ARM", "FREE", "DATA", "RESET", "HELP", "STATUS", "ALARMS_RESET" };
//const byte CommandCount = 7;
//const char Arr0[] = "ARM";
//const char Arr1[] = "FREE";
//const char Arr2[] = "DATA";
//const char Arr3[] = "RESET";
//const char Arr4[] = "HELP";
//const char Arr5[] = "STATUS";
//const char Arr6[] = "ALARMS_RESET";
//
//
//const char* const CommandsList[CommandCount] PROGMEM = // an array list of commads for smart home
//{ Arr0, Arr1, Arr2, Arr3, Arr4, Arr5, Arr6 };


enum TARSmartHomeCommads {
  shARM = 0,
  shFREE = 1,
  shDATA = 2,
  shRESET = 3,
  shHELP = 4,
  shSTATUS  = 5,
  shALARMS_RESET = 6,
};

//const float AlarmHiMoveDetect = 200.0F;
//const float NoAlarmHiMoveDetect = 2000.0F;

//  Variables
// Создать объект программного последовательного порта для связи с SIM900
// Tx и Rx SIM900 подключены к выводам 7 и 8 ArduinoUNO (10,11 for Arduino Mega)

#define PLATFORM_MEGA = true;// true - debug version/ false - release version

#ifdef PLATFORM_UNO
SoftwareSerial mySerial(7, 8);
#else

#ifdef PLATFORM_MEGA
SoftwareSerial mySerial(10, 11); //RX/TX
#else
#endif


#endif


bool Armed = false;
const byte ROOMSCOUNT = 9;//
const char* SmartHomeRoomsCaptions[ROOMSCOUNT]  = // an array of placement captions in smart home
{ "Hall", "Storage", "BedRoom", "OutDoor", "Attic", "BathRoom", "Kitchen", "OutsideGate", "HallWay" };

// Sensors
const byte SENSORSCOUNT = 11;//50;
TARSensor Sensors[SENSORSCOUNT] = {
  //  Analog inputs
  // Name Placement Digital Unit Period Pin LowValue HiValue
  TARSensor("Knock0", 0, stAnalog, "", 0, A0, -1.0F, 200.0F, Armed),
  TARSensor("Move0", 0, stAnalog, "", 00, A1, -1.0F, 200.0F, Armed),
  TARSensor("Temp0", 3, stAnalog, "C", 00, A2, 18.0F, 30.0F, true),// outdoor
  TARSensor("---", 0, stAnalog, "", 00, A3, 0.0F, 1024.0F, true),//----------------------------------
  TARSensor("Sound0", 0, stAnalog, "dB", 00, A4, 0.0F, 850.0F, true),
  TARSensor("Fire0", 0, stAnalog, "", 00, A5, 0.0F, 700.0F, true),
  //  Digital inputs
  TARSensor("Temp", 0, stTemperature, "C", 500, 4, 15.0F, 32.0F, true), // hall
  TARSensor("Humidity0", 0, stDHTHumidity, "%", 500, 4, 40.0F, 70.0F, true), // hall
  TARSensor("Ligting0", 3, stDigital, "", 0, 2, 0.0F, 1.0F, true),
  TARSensor("Gercon0", 0, stDigital, "", 0, 5, 0.0F, 1.0F, true), // hall
  TARSensor("Gercon1", 1, stDigital, "", 0, 6, 0.0F, 1.0F, true), // storage
  //  // 2 connect & correct
  //  // Smoke
  //  Sensors[8] = new TARSensor("SmokeSensor", PlacementCaptions[0], true, "", 100, A0, 0.0F, 1.0F,  0.0F, 1.0F);
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

void InitSensors()
{
  Sensors[0].onRead = ARAnalogSensorRead;
  Sensors[1].onRead = ARAnalogSensorRead;
  Sensors[2].onRead = ARAnalogTemperatureSensorRead;
  Sensors[3].onRead = ARAnalogSensorRead;
  Sensors[4].onRead = ARAnalogSensorRead;
  Sensors[5].onRead = ARAnalogSensorRead;
  Sensors[6].onRead = DHTTemperatureRead;
  Sensors[7].onRead = DHTHumidityRead;
  Sensors[8].onRead = ARDigitalSensorRead;
  Sensors[9].onRead = ARDigitalSensorRead;
  Sensors[10].onRead = ARDigitalSensorRead;

  Sensors[0].onHi = Alarm;
  Sensors[1].onHi = Alarm;
  Sensors[2].onHi = Alarm;
  Sensors[3].onHi = Alarm;
  Sensors[4].onHi = Alarm;
  Sensors[5].onHi = Alarm;
  Sensors[6].onHi = Alarm;
  Sensors[7].onHi = Alarm;
  Sensors[8].onHi = Alarm;
  Sensors[9].onHi = Alarm;
  Sensors[10].onHi = Alarm;

  //Sensors[0].onLow = Alarm;
  //Sensors[1].onLow = Alarm;
  Sensors[2].onLow = Alarm;
  //Sensors[3].onLow = Alarm;
  //Sensors[4].onLow = Alarm;
  //Sensors[5].onLow = Alarm;
  Sensors[6].onLow = Alarm;
  //Sensors[7].onLow = Alarm;
}

const byte ACTUATORSCOUNT = 4;
// 2 correct
//ШИМ: пины 2–13 и 44–46
TARActuator Actuators[ACTUATORSCOUNT] = {
  //Name,  Placement,  ActuatorType, Period, Pin, LowValue, HiValue,  Active
  TARActuator("Vent0", 0, atAnalog, 0, 3, 0.0F, 1023.0F, true),
  TARActuator("Ligth0", 0, atDigital, 0, 2, 0.0F, 1023.0F, true),
  TARActuator("FireBeep0", 0, atDigital, 0, 13, 0.0F, 1023.0F, true),
  TARActuator("Valve0", 0, atAnalog, 0, 44, 0.0F, 1023.0F, true),
};

void InitActuators()
{
  Actuators[0].onRun = ARAnalogActuatorRun;
  Actuators[1].onRun = ARDigitalActuatorRun;
  Actuators[2].onRun = ARDigitalActuatorRun;
  Actuators[3].onRun = ARAnalogActuatorRun;
  //  Actuators[4].onRun = null;

}




void setup()
{
  //analogReference(INTERNAL);
  Serial.begin(BAUD);//9600

  while (!Serial)
  {
    // wait for serial port to connect.
  }

  InitSMSModem();
  //SendEmail("", "");
  //delay(3000);
  //вернуть
  InitSensors();
  InitActuators();
}

void loop()
{

  //on timer1
  //вернуть
  ReadSensors();

  //on timer2
  //вернуть
  RunActuators();

  // on timer3
  //вернуть
  CheckSMSCommads();

  if (millis() - timer >= PERIOD) {
    // ваше действие
    timer += PERIOD;
  }
}

void InitSMSModem()
{
  // Начать последовательную связь Arduino и SIM900
  SIM900power();
  //delay(2000);

  mySerial.begin(BAUD);//9600
  Serial.println(F("Initializing..."));
  SIM900Restart();
  //delay(2000);
  while (!mySerial.available()) {           // Зацикливаем и ждем инициализацию SIM800L
    mySerial.println("AT");                  // Отправка команды AT
    delay(1000);
    updateSerial();
    Serial.println(F("Connecting..."));         // Печатаем текст
  }
  //mySerial.println(F("AT+CSCS=\"GSM\""));
  //delay(1000);
  //updateSerial();
  Serial.println(F("Connected!"));            // Печатаем текст
  mySerial.println(F("AT+CMGF=1"));           // Настройка текствого режима
  delay(1000);                             // Пауза
  updateSerial();
  mySerial.println(F("AT+CNMI=1,2,0,0,0"));   // Отправка команды AT+CNMI=1,2,0,0,0
  delay(1000);                             // Пауза
  updateSerial();
  mySerial.println(F("AT+CMGL=\"REC UNREAD\""));
  delay(1000);
  updateSerial();
  SendSMS(F("Ready"), OwnerPhoneNumber);
}

void RunActuators()
{
  for (int i = 0; i < ACTUATORSCOUNT; i++)
  {
    Actuators[i].Run();

#ifdef DEBUG
    Serial.println(String(i) + ". " + SmartHomeRoomsCaptions[Actuators[i].Placement] + F(": Actuator") +
                   Actuators[i].Name + ": " + Actuators[i].Value);
#else

#endif

    delay(1000);
  }
}

void ReadSensors()
{
  for (int i = 0; i < SENSORSCOUNT; i++)
  {
    Sensors[i].Read();

#ifdef DEBUG
    Serial.println(String(i) + ". " + SmartHomeRoomsCaptions[Sensors[i].Placement] + F(": Sensor") +
                   Sensors[i].Name + ": " + Sensors[i].Value + Sensors[i].Unit);
#else

#endif

    delay(1000);
  }
}


void CheckSMSCommads()
{
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
    SendSMS("111111111111111111111111111111111111111111111111111111111111", OwnerPhoneNumber);
     
    while (mySerial.available()) {              // Проверяем, есть ли еще данные.
      inputString += (char)mySerial.read();                // Записываем считанный байт в массив inputString
      delay(10);
    }    
#ifdef DEBUG
    Serial.println(inputString);
#else

#endif

    // Отправка в "Мониторинг порта" считанные данные
    inputString.toUpperCase();                  // Меняем все буквы на заглавные

    if ((inputString.indexOf(F("+CIEV:")) > -1) && (inputString.indexOf(F("+CMT:")) > -1)
        && (inputString.indexOf(OwnerPhoneNumber) > -1))
    {
      if (inputString.indexOf(CommandsList[shDATA]) > -1) {     // Проверяем полученные данные
        if (Armed)
          Message = F("ARMED");
        else
          Message = F("DISARMED");
        SendSMS(Message, OwnerPhoneNumber); // send SMS

        for (int i = 0; i < SENSORSCOUNT; i++)
        {
          Sensors[i].Read();
          Message = String(i) + ". " + SmartHomeRoomsCaptions[Sensors[i].Placement] + F(": Sensor")
                    + Sensors[i].Name + ": " + Sensors[i].Value + Sensors[i].Unit;
          Serial.println(Message);
          SendSMS(Message, OwnerPhoneNumber); // send SMS
          // delay(1000);
        }

      }
      //постановка на охрану
      if (inputString.indexOf(CommandsList[shARM]) > -1) {     // Проверяем полученные данные
        Armed = true;
        for (int i = 0; i < SENSORSCOUNT; i++)
          Sensors[i].Armed = Armed;

        Message = F("ARMED");
        Serial.println(Message);
        SendSMS(Message, OwnerPhoneNumber); // send SMS
      }
      // снятие с охраны
      if (inputString.indexOf(CommandsList[shFREE]) > -1) {     // Проверяем полученные данные
        Armed = false;
        Sensors[0].Armed = Armed;
        Sensors[1].Armed = Armed;
        Message = F("DISARMED");
        Serial.println(Message);
        SendSMS(Message, OwnerPhoneNumber); // send SMS
      }
      // Show Commands List
      if (inputString.indexOf(CommandsList[shHELP]) > -1) {     // Проверяем полученные данные
        Message = "";
        for (int i = 0; i < CommandCount; i++)
        {
          Message += String(CommandsList[i]) + "\n";
        }
        Serial.println(Message);
        SendSMS(Message, OwnerPhoneNumber); // send SMS
      }
      // STATUS
      if (inputString.indexOf(CommandsList[shSTATUS]) > -1) {     // Проверяем полученные данные
        if (Armed)
          Message = F("ARMED");
        else
          Message = F("DISARMED");
        SendSMS(Message, OwnerPhoneNumber); // send SMS

        for (int i = 0; i < SENSORSCOUNT; i++)
        {
          Sensors[i].Read();
          Message = String(i) + ". " + SmartHomeRoomsCaptions[Sensors[i].Placement] + F(": Sensor")
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
        }
      }
      // Alarm reset
      if (inputString.indexOf(CommandsList[shALARMS_RESET]) > -1) {     // Проверяем полученные данные
        for (int i = 0; i < SENSORSCOUNT; i++)
        {
          Sensors[i].LowAlarmed = false;
          Sensors[i].HiAlarmed = false;
          Message = F("Alarms reseted");
          Serial.println(Message);
          SendSMS(Message, OwnerPhoneNumber); // send SMS
        }
      }

      // Controller reset
      if (inputString.indexOf(CommandsList[shRESET]) > -1) {     // Проверяем полученные данные
        resetFunc(); //вызываем reset
      }
      delay(1000);
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
  // if UNO???
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  delay(1000);
  digitalWrite(7, HIGH);
  delay(2000);
  digitalWrite(7, LOW);
  delay(3000);
  //resetFunc(); //вызываем reset
}



void updateSerial()
{
  delay(500);
  //  while (Serial.available())
  //  {
  //    // Пересылка того, что было получено с аппаратного последовательного порта,
  //    // на программный последовательный порт
  //    mySerial.write(Serial.read());
  //  }
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
#ifdef DEBUG
  Serial.print(F("AT+CMGS=\""));
  Serial.println( phone + "\"");
  delay(500);
  Serial.println(text);
  delay(500);
  Serial.println(F("SMS send complete"));
  delay(500);
#else
  mySerial.print(F("AT+CMGS=\""));
  mySerial.println(phone + "\"");
  mySerial.print(text);
  mySerial.print((char)26);
  Serial.println(F("SMS send complete"));
#endif
}

void SendEmail(String text, String Mail)  // Процедура Отправка SMS
{
#ifdef DEBUG
  //
  //Электронный адрес  Полное имя почтового ящика, включая логин, @ и домен
  //Сервер входящей почты (IMAP- и POP3-сервера)  IMAP-сервер — imap.mail.ru
  //POP3-сервер — pop.mail.ru
  //Сервер исходящей почты (SMTP-сервер)  smtp.mail.ru
  //Имя пользователя
  //Полное имя почтового ящика, включая логин, @ и домен
  //
  //Пароль  Пароль, который вы используете для входа в почтовый ящик
  //Порт  IMAP — 993 (протокол шифрования SSL/TLS)
  //POP3 — 995 (протокол шифрования SSL/TLS)
  //SMTP — 465 (протокол шифрования SSL/TLS)
  //Аутентификация  Обычный пароль (без шифрования)

  //  AT + CREG = 1//
  //              AT + CREG ?//
  //              Команда проверки подключения модуля к GPRS - сети, которую при ответе COMMAND NO RESPONSE необходимо постоянно повторять
  ///     AT + CGATT = 1//
  //        AT + CGATT ?//
  //       Подключаемся к точке доступа оператора связи. Для Билайн ://
  //     AT + CGDCONT = 1, "IP", "internet.beeline.ru"//
  //         AT + CSTT = "internet.beeline.ru", "", ""//
  //       Установка интернет -соединения ://

  //Последовательность ввода AT команд с описанием
  //Настройки интернет соединения
  //mySerial.print(F("AT+SAPBR=1,1"));
  delay(100);
  updateSerial();
  //  mySerial.print(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"")); // задаем команду выхода в интернет
  mySerial.print(F("AT+CGATT=1"));
  mySerial.print(F("AT+CGATT?"));
  updateSerial();
  delay(100);
  //  mySerial.print(F("AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\"")); // - настройки APN на примере МТС
  //  delay(100);
  //  mySerial.print(F("AT+SAPBR=3,1,\"USER\",\"mts\"")); // - имя пользователя
  //  delay(100);
  //  mySerial.print(F("AT+SAPBR=3,1,\"PWD\",\"mts\"")); // - пароль

  mySerial.print(F("AT+CGDCONT = 1, \"IP\", \"internet.mts.ru\""));
  mySerial.print(F("AT+CSTT = \"internet.mts.ru\", \"mts\", \"mts\"")); // - пароль
  updateSerial();
  delay(100);
  updateSerial();
  //  mySerial.print(F("AT+SAPBR=1,1")); // - установить GPRS соединение
  //  mySerial.print(F("AT+SAPBR=2,1")); // - отобразить IP адрес соединения (не обязателно)
  mySerial.print(F("AT+CGACT=1,1")); // - установить GPRS соединение
  mySerial.print(F("AT+CGACT=2,1")); // - отобразить IP адрес соединения (не обязателно)
  delay(100);
  updateSerial();






  //Настройки для отправки e-mail :
  mySerial.print(F("AT+EMAILCID=1")); // - Установка CID параметра для email сессии.
  mySerial.print(F("AT+EMAILTO=30")); // - Установка таймаута для SMTP и POP серверов.
  mySerial.print(F("AT+SMTPSRV=\"smtp.mail.ru\", 465")); // - Установка адреса и порта SMTP почтового сервера.
  mySerial.print(F("AT+SMTPAUTH=1,\"o_torel@mail.ru\", \"Temple0f1ce\"")); // - Аутентификация (e-mail адрес, и пароль от e-mail)
  mySerial.print(F("AT+SMTPFROM=\"o_torel@mail.ru\", \"Arduino uno\"")); // - от кого письмо (почтовый адрес, имя отправителя)
  mySerial.print(F("AT+SMTPRCPT=0,0,\"o_torel@mail.ru\", \"Atmega328\"")); // - кому письмо (почтовый адрес, имя получателя)
  mySerial.print(F("AT+SMTPSUB= \"First message\"")); // // тема письма
  mySerial.print(F("AT+SMTPBODY= 19")); // задаем сколько символов в письме
  //После получение ответа от модуля DOWNLOAD.
  //вводим текст письма длиной 19 символов.
  //Для отправки в конце сообщения отправляем символ SUB ( (char)26 или Cntrl+Z)
  mySerial.print(F("hello new message"));
  mySerial.print((char)26);
  mySerial.print(F("AT+SMTPSEND")); //- Отправка письма.
  //Ответы:
  //1 - письмо успешно отправлено
  //Некоторые ошибки:
  //61 - ошибка соединения
  //63 - ошибка подключения SMTP TCP
  //64 - время сессии истекло
  //67 - ошибка аутентификации
  mySerial.print(F("AT+SAPBR=0,1")); //- разорвать GPRS соединение



  //mySerial.println(phone + "\"");
  //mySerial.print(text);
  Serial.println(F("Mail send complete"));
#else


#endif
}

void SendHTTPServer(String text, String HTTP)  // Процедура Отправка SMS
{
#ifdef DEBUG
  //
  //Электронный адрес  Полное имя почтового ящика, включая логин, @ и домен
  //Сервер входящей почты (IMAP- и POP3-сервера)  IMAP-сервер — imap.mail.ru
  //POP3-сервер — pop.mail.ru
  //Сервер исходящей почты (SMTP-сервер)  smtp.mail.ru
  //Имя пользователя
  //Полное имя почтового ящика, включая логин, @ и домен
  //
  //Пароль  Пароль, который вы используете для входа в почтовый ящик
  //Порт  IMAP — 993 (протокол шифрования SSL/TLS)
  //POP3 — 995 (протокол шифрования SSL/TLS)
  //SMTP — 465 (протокол шифрования SSL/TLS)
  //Аутентификация  Обычный пароль (без шифрования)

  //AT+CREG=1
  //AT+CREG?
  //Команда проверки подключения модуля к GPRS-сети, которую при ответе COMMAND NO RESPONSE необходимо постоянно повторять
  //AT + CGATT=1
  //AT + CGATT?
  //Подключаемся к точке доступа оператора связи. Для Билайн:
  //AT + CGDCONT = 1, "IP", "internet.beeline.ru"
  //AT + CSTT = "internet.beeline.ru","", ""
  //Установка интернет-соединения:
  //AT + CGACT = 1,1



  //Последовательность ввода AT команд с описанием
  //Настройки интернет соединения
  mySerial.print(F("AT+SAPBR=1,1"));

  mySerial.print(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"")); // задаем команду выхода в интернет
  delay(100);
  mySerial.print(F("AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\"")); // - настройки APN на примере МТС
  delay(100);
  mySerial.print(F("AT+SAPBR=3,1,\"USER\",\"mts\"")); // - имя пользователя
  delay(100);
  mySerial.print(F("AT+SAPBR=3,1,\"PWD\",\"mts\"")); // - пароль
  delay(100);
  updateSerial();
  mySerial.print(F("AT+SAPBR=1,1")); // - установить GPRS соединение
  mySerial.print(F("AT+SAPBR=2,1")); // - отобразить IP адрес соединения (не обязателно)
  delay(100);
  updateSerial();
  //Настройки для отправки e-mail :
  mySerial.print(F("AT+EMAILCID=1")); // - Установка CID параметра для email сессии.
  mySerial.print(F("AT+EMAILTO=30")); // - Установка таймаута для SMTP и POP серверов.
  mySerial.print(F("AT+SMTPSRV=\"smtp.mail.ru\", 465")); // - Установка адреса и порта SMTP почтового сервера.
  mySerial.print(F("AT+SMTPAUTH=1,\"o_torel@mail.ru\", \"Temple0f1ce\"")); // - Аутентификация (e-mail адрес, и пароль от e-mail)
  mySerial.print(F("AT+SMTPFROM=\"o_torel@mail.ru\", \"Arduino uno\"")); // - от кого письмо (почтовый адрес, имя отправителя)
  mySerial.print(F("AT+SMTPRCPT=0,0,\"o_torel@mail.ru\", \"Atmega328\"")); // - кому письмо (почтовый адрес, имя получателя)
  mySerial.print(F("AT+SMTPSUB= \"First message\"")); // // тема письма
  mySerial.print(F("AT+SMTPBODY= 19")); // задаем сколько символов в письме
  //После получение ответа от модуля DOWNLOAD.
  //вводим текст письма длиной 19 символов.
  //Для отправки в конце сообщения отправляем символ SUB ( (char)26 или Cntrl+Z)
  mySerial.print(F("hello new message"));
  mySerial.print((char)26);
  mySerial.print(F("AT+SMTPSEND")); //- Отправка письма.
  //Ответы:
  //1 - письмо успешно отправлено
  //Некоторые ошибки:
  //61 - ошибка соединения
  //63 - ошибка подключения SMTP TCP
  //64 - время сессии истекло
  //67 - ошибка аутентификации
  mySerial.print(F("AT+SAPBR=0,1")); //- разорвать GPRS соединение



  //mySerial.println(phone + "\"");
  //mySerial.print(text);
  Serial.println(F("Mail send complete"));



#else
  mySerial.print(F("AT+CMGS=\""));
  //mySerial.println(phone + "\"");
  mySerial.print(text);
  Serial.println(F("SMS send complete"));
#endif
}



void Alarm(TARSensor & Sensor)
{
  String Message = String (SmartHomeRoomsCaptions[Sensor.Placement]) + F(": Sensor") + String(Sensor.Name + ": " +
                   Sensor.Value + Sensor.Unit  + F("\n"));

#ifdef DEBUG
  SendSMS("ALARM: " + Message, OwnerPhoneNumber);
  Serial.println("ALARM: " + Message);
#else
  SendSMS("ALARM: " + Message, OwnerPhoneNumber);
#endif
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
