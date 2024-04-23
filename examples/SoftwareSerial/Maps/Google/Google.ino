// ПРИМЕР ВЫВОДИТ ССЫЛКУ НА КООРДИНАТЫ:           //
                                                  //
const uint8_t pinRX = 8;                          //  Определяем вывод RX (программного UART) на плате Arduino к которому подключён вывод TX модуля. Номер вывода можно изменить.
const uint8_t pinTX = 9;                          //  Определяем вывод TX (программного UART) на плате Arduino к которому подключён вывод RX модуля. Номер вывода можно изменить.
                                                  //
#include <SoftwareSerial.h>                       //  Подключаем библиотеку для работы с программным UART, до подключения библиотеки iarduino_GPS_NMEA.
#include <iarduino_GPS_NMEA.h>                    //  Подключаем библиотеку для расшифровки строк протокола NMEA получаемых по UART.
                                                  //
SoftwareSerial    SerialGPS(pinRX, pinTX);        //  Объявляем объект SerialGPS для работы с функциями и методами библиотеки SoftwareSerial, указав выводы RX и TX Arduino.
iarduino_GPS_NMEA gps;                            //  Объявляем объект gps для работы с функциями и методами библиотеки iarduino_GPS_NMEA.
                                                  //
void setup(){                                     //
     Serial.begin(9600);                          //  Инициируем работу с аппаратной шиной UART для вывода данных в монитор последовательного порта на скорости 9600 бит/сек.
     SerialGPS.begin(9600);                       //  Инициируем работу с программной шиной UART для получения данных от GPS модуля на скорости 9600 бит/сек.
     gps.begin(SerialGPS);                        //  Инициируем расшифровку строк NMEA указав объект используемой шины UART (вместо программной шины, можно указывать аппаратные: Serial, Serial1, Serial2, Serial3).
}                                                 //
                                                  //
void loop(){                                      //
     gps.read();                                  //  Читаем данные (чтение может занимать больше 1 секунды). Функции можно указать массив для получения данных о спутниках.
     if(!gps.errPos){                             //
      Serial.print("https://www.google.ru/maps/");//  Ссылка на google карты:
                                                  //
      Serial.print("place/");                     //  Координаты точки:
      Serial.print(gps.latitude,5);               //  Широта.
      Serial.print(",");                          //  ,
      Serial.print(gps.longitude,5);              //  Долгота.
                                                  //
      Serial.print("?hl=");                       //  Язык:
      Serial.print("ru");                         //  Русский:
                                                  //
      Serial.print("\r\n");                       //
     }else{                                       //
      Serial.println("Нет данных.");              //
      delay(2000);                                //
     }                                            //
}                                                 //
