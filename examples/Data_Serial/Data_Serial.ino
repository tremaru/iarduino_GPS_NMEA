// ПРИМЕР ВЫВОДИТ ДАННЫЕ NMEA ПОЛУЧАЕМЫЕ ПО UART: //  дата, время, координаты, скорость, направление.
                                                  //
#include <iarduino_GPS_NMEA.h>                    //  Подключаем библиотеку для расшифровки строк протокола NMEA получаемых по UART.
iarduino_GPS_NMEA gps;                            //  Объявляем объект gps для работы с функциями и методами библиотеки iarduino_GPS_NMEA.
                                                  //
char* wd[]={"Вс","Пн","Вт","Ср","Чт","Пт","Сб"};  //  Определяем массив строк содержащих по две первых буквы из названий дня недели.
                                                  //
void setup(){                                     //
     Serial.begin(9600);                          //  Инициируем работу с аппаратной шиной UART для вывода данных в монитор последовательного порта на скорости 9600 бит/сек.
     Serial1.begin(9600);                         //  Инициируем работу с аппаратной шиной UART для получения данных от GPS модуля на скорости 9600 бит/сек.
     gps.begin(Serial1);                          //  Инициируем расшифровку строк NMEA указав объект используемой шины UART (вместо аппаратной шины, можно указывать программную).
     gps.timeZone(3);                             //  Указываем часовой пояс (±12 часов), или GPS_AutoDetectZone для автоматического определения часового пояса по долготе.
}                                                 //
                                                  //
void loop(){                                      //
     gps.read();                                  //  Читаем данные (чтение может занимать больше 1 секунды). Функции можно указать массив для получения данных о спутниках.
     /* Время:                                    */  Serial.print(gps.Hours); Serial.print(":"); Serial.print(gps.minutes); Serial.print(":"); Serial.print(gps.seconds); Serial.print(" ");
     /* Дата:                                     */  Serial.print(gps.day  ); Serial.print("."); Serial.print(gps.month  ); Serial.print("."); Serial.print(gps.year   ); Serial.print("г (");
     /* Дополнительные данные даты:               */  Serial.print(wd[gps.weekday]); Serial.print("), UnixTime: "); Serial.print(gps.Unix); Serial.print(". ");
     /* Координаты (широта, долгота, высота):     */  Serial.print("Ш: "); Serial.print(gps.latitude,5); Serial.print("°, Д: "); Serial.print(gps.longitude,5); Serial.print("°, В: "); Serial.print(gps.altitude,1); Serial.print("м. ");
     /* Движение (скорость, курс):                */  Serial.print("Скорость: "); Serial.print(gps.speed); Serial.print("км/ч, "); Serial.print(gps.course); Serial.print("°. ");
     /* Спутники (активные/наблюдаемые):          */  Serial.print("Спутники: "); Serial.print(gps.satellites[GPS_ACTIVE]); Serial.print("/"); Serial.print(gps.satellites[GPS_VISIBLE]); Serial.print(". ");
     /* Геометрический фактор ухудшения точности: */  Serial.print("PDOP: "); Serial.print(gps.PDOP); Serial.print(", HDOP: "); Serial.print(gps.HDOP); Serial.print(", VDOP: "); Serial.print(gps.VDOP); Serial.print(". ");
     /* Ошибка определения времени:               */  if(gps.errTim){Serial.print("Вемя недостоверно. "           );}
     /* Ошибка определения даты:                  */  if(gps.errDat){Serial.print("Дата недостоверна. "           );}
     /* Ошибка позиционирования:                  */  if(gps.errPos){Serial.print("Координаты недостоверны. "     );}
     /* Ошибка определения скорости и курса       */  if(gps.errCrs){Serial.print("Скорость и курс недостоверны. ");}
     /*                                           */  Serial.print("\r\n");
}                                                 //