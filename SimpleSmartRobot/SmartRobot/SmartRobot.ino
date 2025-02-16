// https://microkontroller.ru/arduino-projects/robot-na-arduino-uno-upravlyaemyj-po-wifi/
// https://zen.yandex.ru/media/id/5e6b3431cc34441806da9ce0/wifi-esp01-esp8266-urok-19-arduino-5f3a7d5834c1866c1c25e7b9
//https://blynk.io/en/developers
// https://arduinoplus.ru/7-prilozhenii-arduino-na-android/
// https://arduinoplus.ru/kak-udalenno-upravlyat-arduino-s-blynk/
// https://the-robot.ru/study/nodemcu-blynk-uroki-1-ustrojsva-vyvoda/
// https://virtuino.com/index.php/virtuino/arduino-libraries
// https://microkontroller.ru/arduino-projects/peredacha-dannyh-ot-arduino-uno-na-veb-straniczu-s-pomoshhyu-wifi/
// https://microkontroller.ru/arduino-projects/peredacha-email-s-ispolzovaniem-arduino-i-wifi-modulya/ 
// https://arduinoplus.ru/arduino-wifi/


#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266_SoftSer.h>
#include <BlynkSimpleShieldEsp8266_SoftSer.h>
// Set ESP8266 Serial object
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX
ESP8266 wifi(EspSerial);
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "caa17a11c0124d4083d0eaa995f45917";
#define m11 8
#define m12 9
#define m21 10
#define m22 11
void forward()
{
  digitalWrite(m11, HIGH);
  digitalWrite(m12,LOW);
  digitalWrite(m21,HIGH);
  digitalWrite(m22,LOW);
}
void backward()
{
  digitalWrite(m11, LOW);
  digitalWrite(m12,HIGH);
  digitalWrite(m21,LOW);
  digitalWrite(m22,HIGH);
}
void right()
{
  digitalWrite(m11, HIGH);
  digitalWrite(m12,LOW);
  digitalWrite(m21,LOW);
  digitalWrite(m22,LOW);
}
void left()
{
  digitalWrite(m11, LOW);
  digitalWrite(m12,LOW);
  digitalWrite(m21,HIGH);
  digitalWrite(m22,LOW);
}
void Stop()
{
  digitalWrite(m11, LOW);
  digitalWrite(m12,LOW);
  digitalWrite(m21,LOW);
  digitalWrite(m22,LOW);
}
void setup()
{
  // Set console baud rate
  Serial.begin(9600);
  delay(10);
  // Set ESP8266 baud rate
  // 9600 is recommended for Software Serial
  EspSerial.begin(9600);
  delay(10);
  Blynk.begin(auth, wifi, "username", "password");  // wifi username and password
  pinMode(m11, OUTPUT);
  pinMode(m12, OUTPUT);
  pinMode(m21, OUTPUT);
  pinMode(m22, OUTPUT);
}
BLYNK_WRITE(V1) 
{
  int x = param[0].asInt();
  int y = param[1].asInt();
  // Do something with x and y
/*  Serial.print("X = ");
  Serial.print(x);
  Serial.print("; Y = ");
  Serial.println(y);*/
  if(y>220)
  forward();
  else if(y<35)
  backward();
  else if(x>220)
  right();
  else if(x<35)
  left();
  else
  Stop();
}
void loop()
{
  Blynk.run();
}
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266_SoftSer.h>
#include <BlynkSimpleShieldEsp8266_SoftSer.h>
// Set ESP8266 Serial object
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX
ESP8266 wifi(EspSerial);
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "caa17a11c0124d4083d0eaa995f45917";
#define m11 8
#define m12 9
#define m21 10
#define m22 11
void forward()
{
  digitalWrite(m11, HIGH);
  digitalWrite(m12,LOW);
  digitalWrite(m21,HIGH);
  digitalWrite(m22,LOW);
}
void backward()
{
  digitalWrite(m11, LOW);
  digitalWrite(m12,HIGH);
  digitalWrite(m21,LOW);
  digitalWrite(m22,HIGH);
}
void right()
{
  digitalWrite(m11, HIGH);
  digitalWrite(m12,LOW);
  digitalWrite(m21,LOW);
  digitalWrite(m22,LOW);
}
void left()
{
  digitalWrite(m11, LOW);
  digitalWrite(m12,LOW);
  digitalWrite(m21,HIGH);
  digitalWrite(m22,LOW);
}
void Stop()
{
  digitalWrite(m11, LOW);
  digitalWrite(m12,LOW);
  digitalWrite(m21,LOW);
  digitalWrite(m22,LOW);
}
void setup()
{
  // Set console baud rate
  Serial.begin(9600);
  delay(10);
  // Set ESP8266 baud rate
  // 9600 is recommended for Software Serial
  EspSerial.begin(9600);
  delay(10);
  Blynk.begin(auth, wifi, "username", "password");  // wifi username and password
  pinMode(m11, OUTPUT);
  pinMode(m12, OUTPUT);
  pinMode(m21, OUTPUT);
  pinMode(m22, OUTPUT);
}
BLYNK_WRITE(V1) 
{
  int x = param[0].asInt();
  int y = param[1].asInt();
  // Do something with x and y
/*  Serial.print("X = ");
  Serial.print(x);
  Serial.print("; Y = ");
  Serial.println(y);*/
  if(y>220)
  forward();
  else if(y<35)
  backward();
  else if(x>220)
  right();
  else if(x<35)
  left();
  else
  Stop();
}
void loop()
{
  Blynk.run();
}
