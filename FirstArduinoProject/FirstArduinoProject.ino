// библиотека для работы с датчиками серии DHT
#include <TroykaDHT.h>
// создаём объект класса DHT
// передаём номер пина к которому подключён датчик и тип датчика
// типы сенсоров: DHT11, DHT21, DHT22
#include <Wire.h>
#include <LiquidCrystal_I2C.h>




#include <SoftwareSerial.h>

// Создать объект программного последовательного порта для связи с SIM900
// Tx и Rx SIM900 подключены к выводам 7 и 8 Arduino
SoftwareSerial mySerial(7, 8);

LiquidCrystal_I2C lcd(0x20, 16, 2); // Устанавливаем дисплей 3F

#define NUM_KEYS 9
int  adc_key_val[NUM_KEYS] = {20, 120, 230, 350, 460, 580, 700, 810, 920};


DHT dht(4, DHT11);

const int KeyboardPort = A3;

const int heaterRelay = 6;
const int ledPinGreen = 8;
const int ledPinRed = 10;
const int ledPinYellow = 12;
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 100;  // threshold value to decide when the detected sound is a knock or not
const int MoveDetectorPort = A1;
const int ScreenCount = 4;

const float temperatureSetting = 25.0;

// these variables will change:
int sensorReading = 0;      // variable to store the value read from the sensor pin
int ledState = LOW;         // variable used to store the last LED status, to toggle the light


#define PERIOD 1000*60*60 // 1 hour
uint32_t timer = 0;

struct RoomInfo {
  int RoomID;
  float Temperature;
  float Humidity;
};

enum EnumButtonValue {
  btnNone = 0,
  btnUp = 1,
  btnDown = 3,
  btnLeft = 2,
  btnRigth = 4,
  btnA = 5,
  btnB = 7,
  btnC = 6,
  btnD = 8
};

void ShowMainMenuDisplay();
void ShowSetupDisplay();
void ShowWelcomeDisplay();
void ShowAlarmDisplay();


int getKey(int keyPin);


int SelectedScreen = 0;
int CurentRow = 0;
void (*P[ScreenCount])(int) = {ShowWelcomeDisplay, ShowMainMenuDisplay, ShowSetupDisplay, ShowAlarmDisplay};
EnumButtonValue PressedButton = btnNone;
EnumButtonValue oldPressedButton = btnNone;



void setup()
{

  lcd.init();
  //lcd.autoscroll();
  lcd.backlight();// Включаем подсветку дисплея
  lcd.clear();
  //lcd.setCursor(8, 1);

  pinMode(ledPinGreen, OUTPUT); // declare the ledPin as as OUTPUT
  pinMode(ledPinRed, OUTPUT); // declare the ledPin as as OUTPUT
  pinMode(ledPinYellow, OUTPUT); // declare the ledPin as as OUTPUT
  pinMode(heaterRelay, OUTPUT); // declare the ledPin as as OUTPUT
  pinMode(KeyboardPort, INPUT);

  Serial.begin(9600);       // use the serial port
  dht.begin();
  (*P[SelectedScreen])(SelectedScreen);


  // Начать последовательную связь Arduino и SIM900
  SIM900power();
  mySerial.begin(9600);  
  Serial.println("Initializing...");
  delay(1000);
  mySerial.println("AT+CSCS=\"GSM\"");
  updateSerial();

  // Рукопожатие с SIM900
  mySerial.println("AT");
  updateSerial();
  // Тест качества сигнала, диапазон значений 0-31, 31 - лучший
  //AT+CSQ – проверить «уровень сигнала» - первое число – это уровень в дБ,
  //он должен быть выше 5. Чем выше, тем лучше.
  mySerial.println("AT+CSQ");
  updateSerial();
  // Прочитать информацию о SIM карте, чтобы убедиться, что SIM карта подключена
  mySerial.println("AT+CCID");
  updateSerial();
  // Проверяем, зарегистрирован ли модуль в сети
  //AT+CREG? – убедиться, что вы зарегистрированы в сети. Второе число должно быть 1 или 5.
  //1 означает, что вы зарегистрированы в домашней сети, а 5 – в роуминге.
  //Другие цифры, отличающиеся этих двух, означают, что вы не зарегистрированы ни в одной сети.
  mySerial.println("AT+CREG?");
  updateSerial();

  //ATI – получить имя и версию платы расширения
  mySerial.println("ATI");
  updateSerial();
  //AT+COPS? – проверить, что вы подключены к сети
  mySerial.println("AT+COPS?");
  updateSerial();
  //AT+COPS=? – вернуть список операторов, присутствующих в сети
  //Каждая скобка — это сеть. Первая цифра — это статус этой сети. 0 — неизвестен,
  //1 — доступен для регистрации, 2 — сеть соответствующая оператору сим карты,
  //3 — заблокированная сеть. Остальные параметры сети — это их идентификаторы.
  //Если вы увидели свою сеть (первая цифра = 2).
  mySerial.println("AT+COPS=?");
  updateSerial();
  //AT + CBC – вернуть состояние литий -полимерного аккумулятора.
  //Второе число является процентом от полного заряда (в данном случае это 93 % ),
  //а третье число является фактическим напряжением в мВ (в данном случае 3, 877 В)
  mySerial.println("AT+CBC");
  updateSerial();
  //delay(20000);
  mySerial.println("ATD+79105544321;");//mySerial.println("ATD++79105544321;");
  updateSerial();
  delay(30000); // ждать 20 секунд...
  mySerial.println("ATH"); // повесить трубку
  updateSerial();




  // Настройка текствого режима
  mySerial.println("AT+CMGF=1");
  updateSerial();
  
  // замените ZZ кодом страны, а xxxxxxxxxxx номером телефона для отправки sms
  //mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");
  mySerial.println("AT+CMGS=?");
  updateSerial();
  mySerial.println("AT+CMGS=\"+79105544321\"");
  updateSerial();
  // текстовый контент
  mySerial.print("Asteron robotics\nTemperature: ");
  updateSerial();
  mySerial.write(26);

  // Решает, как следует обрабатывать новые входящие SMS сообщения
  mySerial.println("AT+CNMI=1,2,0,0,0");
  updateSerial();


}

void loop()
{
  updateSerial();
  //RoomInfo Temperature[24 * 100];
  //mySerial.println("ATD+ +79105544321;");
  //updateSerial();
  //delay(20000); // ждать 20 секунд...


  PressedButton = (EnumButtonValue)getKey(KeyboardPort);
  switch (PressedButton)
  {
    case btnNone:

      break;
    case btnUp:
      if (CurentRow > 0) CurentRow--;
      (*P[SelectedScreen])(SelectedScreen);
      break;
    case btnDown:
      CurentRow++;
      (*P[SelectedScreen])(SelectedScreen);
      break;
    case btnLeft:

      break;
    case btnRigth:

      break;
    case btnA:
      if (SelectedScreen == ScreenCount - 1) SelectedScreen = 0; else SelectedScreen++;
      (*P[SelectedScreen])(SelectedScreen);
      break;
    case btnB:

      break;
    case btnC:

      break;
    case btnD:
      if (SelectedScreen == 0) SelectedScreen = ScreenCount - 1; else SelectedScreen--;
      (*P[SelectedScreen])(SelectedScreen);
      break;

    default:
      break;
  }
  //delay(2000);

  ledState = LOW;
  // read the sensor and store it in the variable sensorReading:
  sensorReading = analogRead(knockSensor);

  // if the sensor reading is greater than the threshold:
  if (sensorReading >= threshold) {
    // toggle the status of the ledPin:
    ledState = HIGH;
    // send the string "Knock!" back to the computer, followed by newline
    Serial.println("Knock!");
  }

  //MoveDetector();

  // update the LED pin itself:
  digitalWrite(ledPinGreen, ledState);
  digitalWrite(ledPinRed, ledState);
  digitalWrite(ledPinYellow, ledState);

  //Archive();

  //ledState = !ledState;

  // если светодиод не горит, то зажигаем, и наоборот
  //  if (ledState == LOW)
  //    ledState = HIGH;
  //  else
  //    ledState = LOW;

  // ждём две секунды


  if (millis() - timer >= PERIOD) {
    // ваше действие
    timer += PERIOD;
  }


} // --------------------------------End of Loop----------------------------------------------

void MoveDetector()
{
  if (analogRead(MoveDetectorPort) > threshold)//500
  {
    ledState = HIGH;
    //Сигнал с датчика движения
    Serial.println("Есть движение !!!");
  }
  else
  {
    ledState = LOW;
    //Нет сигнала
    Serial.println("Всё тихо...");
  }
}

void Archive()
{
  int Period = 1000 * 10;

  if (millis() - timer >= Period) {
    // ваше действие

    // считывание данных с датчика
    dht.read();
    // проверяем состояние данных
    switch (dht.getState()) {
      // всё OK
      case DHT_OK:
        // выводим показания влажности и температуры
        Serial.print("Temperature = ");
        Serial.print(dht.getTemperatureC());
        Serial.println(" C \t");
        //lcd.clear();
        //lcd.print("Temperature=");
        //lcd.print(dht.getTemperatureC());
        //lcd.print("C");


        //Serial.print("Temperature = ");
        //Serial.print(dht.getTemperatureK());
        //Serial.println(" K \t");
        //Serial.print("Temperature=");
        //Serial.print(dht.getTemperatureF());
        //Serial.println(" F");
        Serial.print("Humidity=");
        Serial.print(dht.getHumidity());
        Serial.println("%");
        //lcd.setCursor(0, 1);
        //lcd.print("Humidity=");
        //lcd.print(dht.getHumidity());
        //lcd.print("%");

        if (dht.getTemperatureC() < temperatureSetting) //<
          digitalWrite(heaterRelay, HIGH);
        else
          digitalWrite(heaterRelay, LOW);
        break;
      // ошибка контрольной суммы
      case DHT_ERROR_CHECKSUM:
        Serial.println("Checksum error");
        //lcd.clear();
        //lcd.print("Checksum error");
        break;
      // превышение времени ожидания
      case DHT_ERROR_TIMEOUT:
        Serial.println("Time out error");
        //lcd.clear();
        //lcd.print("Time out error");
        break;
      // данных нет, датчик не реагирует или отсутствует
      case DHT_ERROR_NO_REPLY:
        Serial.println("Sensor not connected");
        //lcd.clear();
        //lcd.print("Error");
        //lcd.setCursor(0, 1);
        //lcd.print("Sensor not connected");
        break;
    }
    timer += Period;
  }
}


void ShowMainMenuDisplay()
{
  String Strings[] = {"Setup", "Welcome", "Guard", "Alarm", "Climat"};
  if (CurentRow > 3)CurentRow = 0;
  lcd.clear();
  lcd.print(Strings[CurentRow]);
  lcd.setCursor(0, 1);
  lcd.print(Strings[CurentRow + 1]);

}

void ShowSetupDisplay()
{
  lcd.clear();
  lcd.print("Setup");
  lcd.setCursor(0, 1);
  lcd.print("Temperature = ");
  lcd.cursor();
  lcd.print(temperatureSetting);
}

void ShowWelcomeDisplay()
{
  lcd.clear();
  lcd.print("My smart Home V1.0.0");
  lcd.setCursor(0, 1);
  lcd.print("by Asteron Robotics Evolution");
}

void ShowAlarmDisplay()
{
  lcd.clear();
  lcd.print("Alarm");
  lcd.setCursor(0, 1);
  lcd.print("Knock");
}


int getKey(int keyPin)
{
  int Period = 300 * 1;
  int input = 0;
  if (millis() - timer >= Period) {
    // ваше действие
    input = readMedian(keyPin, 10);
    timer += Period;
  }
  for (int k = 0; k < NUM_KEYS; k++)
    if (input < adc_key_val[k])
      return k;

  return 0;
}



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

// функция считывает аналоговый вход заданное количество раз
// и возвращает медианное отфильтрованное значение
int readMedian (int pin, int samples) {
  // массив для хранения данных
  int raw[samples];
  // считываем вход и помещаем величину в ячейки массива
  for (int i = 0; i < samples; i++) {
    raw[i] = analogRead(pin);
  }
  // сортируем массив по возрастанию значений в ячейках
  int temp = 0; // временная переменная

  for (int i = 0; i < samples; i++) {
    for (int j = 0; j < samples - 1; j++) {
      if (raw[j] > raw[j + 1]) {
        temp = raw[j];
        raw[j] = raw[j + 1];
        raw[j + 1] = temp;
      }
    }
  }
  // возвращаем значение средней ячейки массива
  return raw[samples / 2];
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

/* matherials









  // библиотека для работы с дисплеем
  #include <TroykaTextLCD.h>
  // создаем объект для работы с дисплеем
  TroykaTextLCD lcd;
  // по умолчанию в параметры конструктора дисплея зашиты параметры:
  // выбор шины I²C (&Wire), I²C-адрес и пин подсветки
  // TroykaTextLCD lcd(&Wire, 0x3E, 7);
  // ---------------------------------------------------------------
  #define BPIN A0   // пин к которому подключена клавиатура
  uint8_t AB;       // новое значение номера кнопки
  uint8_t ABOld;    // старое значение  номера кнопки
  uint8_t BT1;      // кол-во нажатий на кнопку №1
  uint8_t BT2;      // кол-во нажатий на кнопку №2
  uint8_t BT3;      // кол-во нажатий на кнопку №3
  uint8_t BT4;      // кол-во нажатий на кнопку №4
  uint8_t BT5;      // кол-во нажатий на кнопку №5

  void setup() {
  lcd.begin(16, 2);
  lcd.setContrast(50);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(BT1);
  lcd.print('-');
  lcd.print(BT2);
  lcd.print('-');
  lcd.print(BT3);
  lcd.print('-');
  lcd.print(BT4);
  lcd.print('-');
  lcd.print(BT5);
  }

  uint8_t GetAB() {                                    // Функция устраняющая дребезг
  static int   count;
  static int   oldKeyValue;                          // предыдущее значение состояния кнопок
  static int   innerKeyValue;
  uint8_t actualKeyValue = (analogRead(BPIN) / 171); // Получаем актуальное состояние
  if (innerKeyValue != actualKeyValue) {             // Пришло значение отличное от предыдущего
    count = 0;                                       // Все обнуляем и начинаем считать заново
    innerKeyValue = actualKeyValue;                  // Запоминаем новое значение
  }
  else {
    count += 1;                                           // Увеличиваем счетчик
  }
  if ((count >= 10) && (actualKeyValue != oldKeyValue)) { // Счетчик преодолел барьер
    oldKeyValue = actualKeyValue;                         // Присваиваем новое значение
  }
  return    oldKeyValue;
  }

  void loop() {
  AB = GetAB();             // читаем значение кнопки
  if (AB == 0) ABOld = 0;   // все кнопки отпущены
  if (AB != ABOld) {        // если нажата одна из кнопок
    ABOld = AB;             // запоминаем значение кнопки
    if (AB == 1) BT1++;     // определяем какая именно кнопка
    if (AB == 2) BT2++;     // была нажата
    if (AB == 3) BT3++;
    if (AB == 4) BT4++;
    if (AB == 5) BT5++;
    lcd.clear();            // обновляем значение
    lcd.setCursor(0, 0);    // нажатий кнопок на дисплее
    lcd.print(BT1);
    lcd.print('-');
    lcd.print(BT2);
    lcd.print('-');
    lcd.print(BT3);
    lcd.print('-');
    lcd.print(BT4);
    lcd.print('-');
    lcd.print(BT5);
  }
  }

*/
