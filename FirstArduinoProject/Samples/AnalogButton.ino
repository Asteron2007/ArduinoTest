#define BPIN A0
uint8_t AB;
uint8_t ABOld;
uint8_t BT1;
uint8_t BT2;
uint8_t BT3;
uint8_t BT4;
uint8_t BT5;

void setup() {
  Serial.begin(9600);
}


// http://codius.ru/articles/Arduino_%D0%9F%D0%BE%D0%B4%D0%BA%D0%BB%D1%8E%D1%87%D0%B5%D0%BD%D0%B8%D0%B5_%D0%BD%D0%B5%D1%81%D0%BA%D0%BE%D0%BB%D1%8C%D0%BA%D0%B8%D1%85_%D0%BA%D0%BD%D0%BE%D0%BF%D0%BE%D0%BA_%D0%BA_%D0%BE%D0%B4%D0%BD%D0%BE%D0%BC%D1%83_%D0%B0%D0%BD%D0%B0%D0%BB%D0%BE%D0%B3%D0%BE%D0%B2%D0%BE%D0%BC%D1%83_%D0%B2%D1%85%D0%BE%D0%B4%D1%83

uint8_t GetAB() {                                           // Функция устраняющая дребезг
  static int   count;
  static int   oldKeyValue;                                 // Переменная для хранения предыдущего значения состояния кнопок
  static int   innerKeyValue;
  uint8_t actualKeyValue = (analogRead(BPIN) / 171);        // Получаем актуальное состояние
  if (innerKeyValue != actualKeyValue) {                    // Пришло значение отличное от предыдущего
    count = 0;                                              // Все обнуляем и начинаем считать заново
    innerKeyValue = actualKeyValue;                         // Запоминаем новое значение
  }
  else {
    count += 1;                                             // Увеличиваем счетчик
  }
  if ((count >= 10) && (actualKeyValue != oldKeyValue)) {   // Счетчик преодолел барьер, можно иницировать смену состояний
    oldKeyValue = actualKeyValue;                           // Присваиваем новое значение
  }
  return    oldKeyValue;
}

void loop() {

  // читаем значение кнопки
  AB = GetAB();

  // все кнопки отпущены
  if (AB == 0) ABOld = 0;

  // клацнули какую то кнопку
  if (AB != ABOld) {
    // запоминаем значение кнопки
    ABOld = AB;
    if (AB == 1) BT1++;
    if (AB == 2) BT2++;
    if (AB == 3) BT3++;
    if (AB == 4) BT4++;
    if (AB == 5) BT5++;
    Serial.print(BT1);
    Serial.print('-');
    Serial.print(BT2);
    Serial.print('-');
    Serial.print(BT3);
    Serial.print('-');
    Serial.print(BT4);
    Serial.print('-');
    Serial.println(BT5);
  }

}
