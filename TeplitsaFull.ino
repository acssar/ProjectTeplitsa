#include <Servo.h>
#include "DHT.h"

DHT dht(2, DHT11);
DHT dht2(3, DHT22);
Servo myservo;
int pos = 0;
int k, fortochka = 0;

unsigned long TimeVent1, TimeVent2; //переменные для millis
int state = 0, kek = 0; // переменные, чтобы не включались одновременно вентилятор и нагреватель
int Tx; //время, на которое будем включать вентилятор
int holodno = 27; // для температуры, пока что int, тупо для проверки кода

void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");

  dht.begin();
  myservo.attach(9);   //моторОкна
  pinMode(7, OUTPUT);  //нагреватель
  pinMode(8, OUTPUT);  //вентилятор
  pinMode(10, OUTPUT); //лампа
  pinMode(11, OUTPUT); //насос
}

void loop() {
  /////////////окно///////////////////////////////////////////////////////////////////////////
  // Температура в теплице
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Temperature: "); //вывод температуры в терминал
    Serial.print(t);
    Serial.println(" *C");
  } //здесь была пропущенна скобка 22.40 30.03
  // Температура за теплицей
  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
  if (isnan(t2) || isnan(h2)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Температура за теплицей: "); //вывод температуры в терминал
    Serial.print(t2);
    Serial.println(" *C");
  }


  if ((t > t2) && (t > 25) && (k == 0) && (fortochka == 0)) // открыть форточку если температура в теплице выше, чем за ней и 25 градусов.
  { for (pos = 0; pos <= 180; pos += 1) { // поворачивает с 0 до 180 градусов
      // in steps of 1 degree
      myservo.write(pos);              // сказать серво повернуться
      delay(15);
    }                       // ждать 0,15с, пока серво поворачивается
    fortochka = 1; // открыта форточка
    t = dht.readTemperature();
    t2 = dht2.readTemperature();
    Serial.print("Температура в теплице: "); //вывод температуры в терминал
    Serial.print(t);
    Serial.println(" *C");
    Serial.print("Температура за теплицей: "); //вывод температуры в терминал
    Serial.print(t2);
    Serial.println(" *C");
  }
  if (fortochka == 1) { //если форточка открыта и температура в теплице больше 25, а на улице меньше оставь так, пока не остынет.
    while ((t > 25) && (t2 <= 25)) {
      digitalWrite(9, LOW);
      t = dht.readTemperature();
      t2 = dht2.readTemperature();
      Serial.print("Температура в теплице: "); //вывод температуры в терминал
      Serial.print(t);
      Serial.println(" *C");
      Serial.print("Температура за теплицей: "); //вывод температуры в терминал
      Serial.print(t2);
      Serial.println(" *C");
    }
    k = 1;
  }
  if ( (k == 1) && ((t <= 25) || (t >= 25)) )
  {
    for (; pos >= 0; pos -= 1) { // идет со 180 до 0 градусов
      myservo.write(pos);              // сказать серво повернуться
      delay(15);                       // ждать 0,15с, пока серво поворачивается
    }
    fortochka = 0;
    k = 0;
    digitalWrite(9, LOW);
    t = dht.readTemperature();
    t2 = dht2.readTemperature();
    Serial.print("Температура в теплице: "); //вывод температуры в терминал
    Serial.print(t);
    Serial.println(" *C");
    Serial.print("Температура за теплицей: "); //вывод температуры в терминал
    Serial.print(t2);
    Serial.println(" *C");
  }
  //////////////////////////////////////////////////////////
  ///////вентилятор/////
  //////////////////////////////////////////////////////////
  t = dht.readTemperature();
  if ((t < holodno) && (kek == 0)) { //если температура такая холодная, что надо включить нагреватель и переменная kek==0(изначально она как раз равна нулю)
    kek = 1; // это и так понятно
    digitalWrite(7, HIGH); // включается нагреватель
    TimeVent1 = millis();
  } //начинается отсчет секунд
  if ((state == 0) && (TimeVent1 > 300000)) { // если некая переменная state=0(изначально равно нулю) и прошло больше пяти минут
    state = 1; //это понятно и так
    TimeVent1 = millis(); // начинается снова отсчет времени
    digitalWrite(7, LOW); // выключается нагреватель
    digitalWrite(8, HIGH);
  } //включается вентилятор
  if ((state == 1) && (TimeVent1 > Tx)) { // если state == 1 и t1>Tx (Tx- время, которое будет работать вентилятор)
    digitalWrite(8, LOW); //выключается вентилятор
    kek = 0; // кеку возвращается ноль
  }
  ///////////////////////////////////////////////////////////
  /////////Лампа///////////////////
  //////////////////////////////////////////////////////////
  int Lumens = analogRead(A2); //считывает аналоговый сигнал (напряжение) с фоторезистора
  //boolean svet, lux = false;
  if (Lumens < 10) {
    digitalWrite(10, HIGH);
  }
  else {
    digitalWrite(10, LOW);
  }
  //////////////////////////////////////////////////////////
  /////////Поливальная система//////////////////////////////
  //////////////////////////////////////////////////////////
  int pochva = analogRead(A3); //считывает аналоговый сигнал (напряжение) с резистора влажности
  for (; pochva > 600;) {
    digitalWrite(10, HIGH);
    pochva = analogRead(A3);
  }
  digitalWrite(10, LOW);
  //////////////////////////////////////////////////////////////
}
