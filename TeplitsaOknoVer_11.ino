#include <Servo.h>
#include "DHT.h"

//#define DHTPIN 2           // пин для датчика
//#define DHTPIN 3
//раскомментировать доступный датчик
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302)       //это вообще все не трогать
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(2, DHT11);
DHT dht2(3, DHT22);
Servo myservo;
int pos = 0;
int k, fortochka = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");

  dht.begin();
  myservo.attach(9);
}

void loop() {
// Температура в теплице
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Temperature: "); //вывод температуры в терминал
    Serial.print(t);
    Serial.println(" *C");} //здесь была пропущенна скобка 22.40 30.03
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
      while ((t > 25) && (t2<=25)) {
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
  if ( (k == 1) && ((t <= 25) || (t>=25)) )
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

}

