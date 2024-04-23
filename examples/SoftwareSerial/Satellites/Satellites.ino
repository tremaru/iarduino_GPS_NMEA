// ПРИМЕР ВЫВОДИТ ДАННЫЕ NMEA ПОЛУЧАЕМЫЕ ПО UART: //  дата, время, координаты, скорость, направление, данные о спутниках.
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
uint8_t i[20][7];                                 //  Объявляем массив для получения данных о 20 спутниках в формате: {ID спутника (1...255), Отношение сигнал/шум (SNR) в дБ, тип навигационной системы (1-GPS/2-Глонасс/3-Galileo/4-Beidou/5-QZSS), Флаг участия спутника в позиционировании (1/0), Угол возвышения спутника (0°-горизонт ... 90°-зенит), Азимут положения спутника (0°...255°), Остаток азимута до 360° }.
char* sa[]={"NoName ","GPS    ","Глонасс","Galileo","Beidou ","QZSS   "}; // Определяем массив строк содержащих названия навигационных систем спутников.
char* wd[]={"Вс","Пн","Вт","Ср","Чт","Пт","Сб"};  //  Определяем массив строк содержащих по две первых буквы из названий дня недели.
                                                  //
void setup(){                                     //
     Serial.begin(9600);                          //  Инициируем работу с аппаратной шиной UART для вывода данных в монитор последовательного порта на скорости 9600 бит/сек.
     SerialGPS.begin(9600);                       //  Инициируем работу с программной шиной UART для получения данных от GPS модуля на скорости 9600 бит/сек.
     gps.begin(SerialGPS);                        //  Инициируем расшифровку строк NMEA указав объект используемой шины UART (вместо программной шины, можно указывать аппаратные: Serial, Serial1, Serial2, Serial3).
     gps.timeZone(3);                             //  Указываем часовой пояс (±12 часов), или GPS_AutoDetectZone для автоматического определения часового пояса по долготе.
}                                                 //
                                                  //
void loop(){                                      //
     gps.read(i);                                 //  Читаем данные с получением информации о спутниках в массив i (чтение может занимать больше 1 секунды). Если указать второй параметр gps.read(i,true); то в массиве окажутся данные только тех спутников, которые принимают участие в позиционировании.
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
                                                  //
     for(uint8_t j=0; j<20; j++){                 //  Проходим по всем элементам массива «i».
        if( i[j][0] ){                            //  Если у спутника есть ID, то выводим информацию о нём:
            if(j<9){Serial.print(" ");}   Serial.print(j+1);         Serial.print(") "  );
            Serial.print("Спутник "    ); Serial.print(sa[i[j][2]]); Serial.print(" "   );
            Serial.print("ID: "        ); Serial.print(   i[j][0] ); Serial.print(". "  );
            Serial.print("Уровень: "   ); Serial.print(   i[j][1] ); Serial.print("дБ. ");
            Serial.print("Возвышение: "); Serial.print(   i[j][4] ); Serial.print("°. " );
            Serial.print("Азимут: "    ); Serial.print(   i[j][5]+i[j][6] ); Serial.print("°. " );
            if(i[j][3]){Serial.print("Участвует в позиционировании.");}
            Serial.print("\r\n");                 //
        }                                         //
    }                                             //
}                                                 //