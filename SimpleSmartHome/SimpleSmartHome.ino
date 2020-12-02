
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
// IRQ
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
#include <math.h>


//#define DEBUG = true;// true - debug version/ false - release version
//#define PLATFORM_UNO;// true - debug version/ false - release version
#define PLATFORM_MEGA = true;// true - debug version/ false - release version

#define PERIOD 1000*60*60 // 1 hour
uint32_t timer = 0;


// Constants
const char* OwnerPhoneNumber  = "+79105544321";
const long BAUD = 9600;//115200;//9600;



const byte CommandCount = 7;
const char* CommandsList[CommandCount]  = // an array list of commads for smart home
{ "ARM", "FREE", "DATA", "RESET", "HELP", "STATUS", "ALARMS_RST" };
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
//SoftwareSerial Serial3(7, 8);
#else

#ifdef PLATFORM_MEGA
//SoftwareSerial Serial3(19, 18); //RX/TX

#else
#endif


#endif
//SoftwareSerial Serial3(14, 15); //RX/TX

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
  //Sensors[2].onRead = ARAnalogTemperatureSensorRead;
  Sensors[2].onRead = AnalogThermisterRead;

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
  Serial.begin(BAUD);//9600
  Serial3.begin(BAUD);//9600

  while (!Serial)
  {
    // wait for serial port to connect.
  }
  while (!Serial3)
  {
    // wait for serial port to connect.
  }
  InitSMSModem();
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
  //RunActuators();

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

  Serial.println(F("Initializing..."));
  SIM900Restart();
  //delay(2000);
  while (!Serial3.available()) {           // Зацикливаем и ждем инициализацию SIM800L
    Serial3.println("AT");                  // Отправка команды AT
    delay(1000);
    Serial.println(F("Connecting..."));         // Печатаем текст
  }
  delay(300);
  Serial.println(F("Connected!"));            // Печатаем текст
  Serial3.println(F("AT+CMGF=1"));           // Настройка текствого режима
  delay(300);                                // Пауза
  //Serial3.println("AT+CSCS=\"GSM\"");        // кодировка текста - GSM
  //delay(300);
  //Serial3.println(F("AT+CNMI=1,2,0,0,0"));   // Отправка команды AT+CNMI=1,2,0,0,0
  Serial3.println(F("AT+CNMI=2,2,0,0,0"));   // Отправка команды AT+CNMI=1,2,0,0,0

  //AT+CNMI=1,2,2,1,0 — вывод принятого sms в терминал.
  delay(300);                             // Пауза
  Serial3.println(F("AT+CMGL=\"REC UNREAD\""));
  delay(300);
  Serial3.println("AT+CMGD=1,4"); //delete all the message in the storage
  delay(300);



  //   Serial3.println("AT+CMGF=1");           // Отправка команды AT+CMGF=1
  //   delay(1000);                             // Пауза
  //   Serial3.println("AT+CNMI=1,2,0,0,0");   // Отправка команды AT+CNMI=1,2,0,0,0
  //   delay(1000);                             // Пауза
  //   Serial3.println("AT+CMGL=\"REC UNREAD\"");


  //  Serial3.println(F("AT+CSCS=\"GSM\""));
  //  delay(1000);
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

  //  if (!Serial3.available())
  //  {
  //    Serial3.println("AT");
  //    delay(1000);
  //    if (!Serial3.available())
  //    {
  //      Serial.println(F("RESET"));
  //      SIM900Restart();
  //    }
  //  }

  if (Serial3.available())// Проверяем, если есть доступные данные
    //while (Serial3.available())// Проверяем, если есть доступные данные
  {
    String inputString;
    String Message = "";
    while (Serial3.available()) {              // Проверяем, есть ли еще данные.
      inputString += (char)Serial3.read();     // Записываем считанный байт в массив inputString
      delay(10);
    }
#ifdef DEBUG
    Serial.println(inputString);
#else
    Serial.println(inputString);
#endif

    // Отправка в "Мониторинг порта" считанные данные
    inputString.toUpperCase();                  // Меняем все буквы на заглавные

    if ((inputString.indexOf(F("+CIEV:")) > -1) && (inputString.indexOf(F("+CMT:")) > -1)
        && (inputString.indexOf(OwnerPhoneNumber) > -1))
    {
      int ind = inputString.indexOf(CommandsList[shDATA]);
      if (ind > -1) {     // Проверяем полученные данные
        if (Armed)
          Message = F("ARMED");
        else
          Message = F("DISARMED");
        SendSMS(Message, OwnerPhoneNumber); // send SMS
        String tmp = inputString.substring(ind + strlen(CommandsList[shDATA]),
                                           ind + strlen(CommandsList[shDATA]) + 3);
        int i =  tmp.toInt();
        //SendSMS(tmp, OwnerPhoneNumber); // send SMS
        //for (int i = 0; i < SENSORSCOUNT; i++)
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
        // if (inputString.indexOf("HELP") > -1) {     // Проверяем полученные данные
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
        delay(1000);

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
          delay(1000);
          SendSMS(Message, OwnerPhoneNumber); // send SMS
          delay(1000);
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
        Serial3.println(F("AT+CMGD=2,4"));
        //Serial3.println("AT+CMGDA=\"DEL ALL\"")
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
  while (Serial.available())
  {
    // Пересылка того, что было получено с аппаратного последовательного порта,
    // на программный последовательный порт
    Serial3.write(Serial.read());
  }
  while (Serial3.available())
  {
    // Пересылка того, что было получено с программного последовательного порта,
    // на аппаратный последовательный порт
    Serial.write(Serial3.read());
  }
}
//
void SendSMS(String text, String phone)  // Процедура Отправка SMS
{
  delay(500);
#ifdef DEBUG
  Serial.print(F("AT+CMGS=\""));
  Serial.println( phone + "\"");
  delay(500);
  Serial.println(text);
  delay(500);
  Serial.println(F("SMS send complete"));
  delay(500);
#else
  Serial3.print(F("AT+CMGS=\""));
  Serial3.println(phone + "\"");
  delay(5000);
  Serial3.print(text);
  delay(5000);
  Serial3.print((char)26);
  delay(5000);
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
  //Serial3.print(F("AT+SAPBR=1,1"));
  delay(100);
  updateSerial();
  //  Serial3.print(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"")); // задаем команду выхода в интернет
  Serial3.print(F("AT+CGATT=1"));
  Serial3.print(F("AT+CGATT?"));
  updateSerial();
  delay(100);
  //  Serial3.print(F("AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\"")); // - настройки APN на примере МТС
  //  delay(100);
  //  Serial3.print(F("AT+SAPBR=3,1,\"USER\",\"mts\"")); // - имя пользователя
  //  delay(100);
  //  Serial3.print(F("AT+SAPBR=3,1,\"PWD\",\"mts\"")); // - пароль

  Serial3.print(F("AT+CGDCONT = 1, \"IP\", \"internet.mts.ru\""));
  Serial3.print(F("AT+CSTT = \"internet.mts.ru\", \"mts\", \"mts\"")); // - пароль
  updateSerial();
  delay(100);
  updateSerial();
  //  Serial3.print(F("AT+SAPBR=1,1")); // - установить GPRS соединение
  //  Serial3.print(F("AT+SAPBR=2,1")); // - отобразить IP адрес соединения (не обязателно)
  Serial3.print(F("AT+CGACT=1,1")); // - установить GPRS соединение
  Serial3.print(F("AT+CGACT=2,1")); // - отобразить IP адрес соединения (не обязателно)
  delay(100);
  updateSerial();






  //Настройки для отправки e-mail :
  Serial3.print(F("AT+EMAILCID=1")); // - Установка CID параметра для email сессии.
  Serial3.print(F("AT+EMAILTO=30")); // - Установка таймаута для SMTP и POP серверов.
  Serial3.print(F("AT+SMTPSRV=\"smtp.mail.ru\", 465")); // - Установка адреса и порта SMTP почтового сервера.
  Serial3.print(F("AT+SMTPAUTH=1,\"o_torel@mail.ru\", \"Temple0f1ce\"")); // - Аутентификация (e-mail адрес, и пароль от e-mail)
  Serial3.print(F("AT+SMTPFROM=\"o_torel@mail.ru\", \"Arduino uno\"")); // - от кого письмо (почтовый адрес, имя отправителя)
  Serial3.print(F("AT+SMTPRCPT=0,0,\"o_torel@mail.ru\", \"Atmega328\"")); // - кому письмо (почтовый адрес, имя получателя)
  Serial3.print(F("AT+SMTPSUB= \"First message\"")); // // тема письма
  Serial3.print(F("AT+SMTPBODY= 19")); // задаем сколько символов в письме
  //После получение ответа от модуля DOWNLOAD.
  //вводим текст письма длиной 19 символов.
  //Для отправки в конце сообщения отправляем символ SUB ( (char)26 или Cntrl+Z)
  Serial3.print(F("hello new message"));
  Serial3.print((char)26);
  Serial3.print(F("AT+SMTPSEND")); //- Отправка письма.
  //Ответы:
  //1 - письмо успешно отправлено
  //Некоторые ошибки:
  //61 - ошибка соединения
  //63 - ошибка подключения SMTP TCP
  //64 - время сессии истекло
  //67 - ошибка аутентификации
  Serial3.print(F("AT+SAPBR=0,1")); //- разорвать GPRS соединение



  //Serial3.println(phone + "\"");
  //Serial3.print(text);
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
  Serial3.print(F("AT+SAPBR=1,1"));

  Serial3.print(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"")); // задаем команду выхода в интернет
  delay(100);
  Serial3.print(F("AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\"")); // - настройки APN на примере МТС
  delay(100);
  Serial3.print(F("AT+SAPBR=3,1,\"USER\",\"mts\"")); // - имя пользователя
  delay(100);
  Serial3.print(F("AT+SAPBR=3,1,\"PWD\",\"mts\"")); // - пароль
  delay(100);
  updateSerial();
  Serial3.print(F("AT+SAPBR=1,1")); // - установить GPRS соединение
  Serial3.print(F("AT+SAPBR=2,1")); // - отобразить IP адрес соединения (не обязателно)
  delay(100);
  updateSerial();
  //Настройки для отправки e-mail :
  Serial3.print(F("AT+EMAILCID=1")); // - Установка CID параметра для email сессии.
  Serial3.print(F("AT+EMAILTO=30")); // - Установка таймаута для SMTP и POP серверов.
  Serial3.print(F("AT+SMTPSRV=\"smtp.mail.ru\", 465")); // - Установка адреса и порта SMTP почтового сервера.
  Serial3.print(F("AT+SMTPAUTH=1,\"o_torel@mail.ru\", \"Temple0f1ce\"")); // - Аутентификация (e-mail адрес, и пароль от e-mail)
  Serial3.print(F("AT+SMTPFROM=\"o_torel@mail.ru\", \"Arduino uno\"")); // - от кого письмо (почтовый адрес, имя отправителя)
  Serial3.print(F("AT+SMTPRCPT=0,0,\"o_torel@mail.ru\", \"Atmega328\"")); // - кому письмо (почтовый адрес, имя получателя)
  Serial3.print(F("AT+SMTPSUB= \"First message\"")); // // тема письма
  Serial3.print(F("AT+SMTPBODY= 19")); // задаем сколько символов в письме
  //После получение ответа от модуля DOWNLOAD.
  //вводим текст письма длиной 19 символов.
  //Для отправки в конце сообщения отправляем символ SUB ( (char)26 или Cntrl+Z)
  Serial3.print(F("hello new message"));
  Serial3.print((char)26);
  Serial3.print(F("AT+SMTPSEND")); //- Отправка письма.
  //Ответы:
  //1 - письмо успешно отправлено
  //Некоторые ошибки:
  //61 - ошибка соединения
  //63 - ошибка подключения SMTP TCP
  //64 - время сессии истекло
  //67 - ошибка аутентификации
  Serial3.print(F("AT+SAPBR=0,1")); //- разорвать GPRS соединение



  //Serial3.println(phone + "\"");
  //Serial3.print(text);
  Serial.println(F("Mail send complete"));



#else
  Serial3.print(F("AT+CMGS=\""));
  //Serial3.println(phone + "\"");
  Serial3.print(text);
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
//SoftwareSerial Serial3(3, 2);                           // Выводы SIM800L Tx & Rx подключены к выводам Arduino 3 и 2
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
//  Serial3.begin(9600);
//
//  bme.begin(0x76);
//
//
//  while(!Serial3.available()){             // Зацикливаем и ждем инициализацию SIM800L
//   Serial3.println("AT");                  // Отправка команды AT
//   delay(1000);                             // Пауза
//   Serial.println("Connecting...");         // Печатаем текст
//   }
//   Serial.println("Connected!");            // Печатаем текст
//   Serial3.println("AT+CMGF=1");           // Отправка команды AT+CMGF=1
//   delay(1000);                             // Пауза
//   Serial3.println("AT+CNMI=1,2,0,0,0");   // Отправка команды AT+CNMI=1,2,0,0,0
//   delay(1000);                             // Пауза
//   Serial3.println("AT+CMGL=\"REC UNREAD\"");
//}
//
//void loop()
//
//{
//  if(Serial3.available()){                       // Проверяем, если есть доступные данные
//      delay(100);                                 // Пауза
//
//      while(Serial3.available()){                // Проверяем, есть ли еще данные.
//      incomingByte = Serial3.read();             // Считываем байт и записываем в переменную incomingByte
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
//          Serial3.println("AT+CMGDA=\"DEL ALL\"");
//          delay(1000);}
//
//          inputString = "";}}
//
//void sms(String text, String phone)  // Процедура Отправка SMS
//{
//  Serial.println("SMS send started");
//  Serial3.println("AT+CMGS=\"" + phone + "\"");
//  delay(500);
//  Serial3.print(text);
//  delay(500);
//  Serial3.print((char)26);
//  delay(500);
//  Serial.println("SMS send complete");
//  delay(2000);
//}





//
//
//
//Картинка показывает, что должно появляться в окне SerialMonitor после того, как вы ввели различные команды.
//
//AT для тестовых примеров:
//AT =====> ESP8266 возвращает OK
//AT + RST =====> ESP8266 рестарт и возврат OK
//AT + GMR =====> ESP8266 возвращает версию AT; SDK version; id; OK
//AT + CWMODE? => ESP8266 возвращает режим работы
//AT + CWLAP ===> ESP8266 возвращает обнаруженные точки доступа
//AT + CIFSR ===> ESP8266 возвращает установленный IP
//AT + CIPMUX = 1 ==> Устанавливает плате ESP8266 режим работы с несколькими соединениями
//AT + CIOBAUD = 9600 ==> Изменить скорость обмена данными  ==> ESP8266 возвращает OK
//AT + CIPSERVER = 1.80 ==> Устанавливает режим  SERVER порт: 4040
//AT + CWMODE = 3 ==> Работа ESP8266 в комбинированном режиме  (точка доступа (2) и сервер (1))
//AT + CWSAP = “Acc_Point_name”, “password”, wifi_Channel, cript # ==> j.
//AT + CWSAP = “ESP_8266_AP,” 1234 “, 3.0
//AT + CWJAP = “SSID”, “password” ==> Подключается к сети WiFi
//* = AT + CWJAP “ROVAI TIMECAP”, “-1 mjr747”
//
//
