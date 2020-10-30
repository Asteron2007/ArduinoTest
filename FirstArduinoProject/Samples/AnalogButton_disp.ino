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
