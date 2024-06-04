//	Библиотека парсинга протокола NMEA.
//  Версия: 1.1.2
//  Последнюю версию библиотеки Вы можете скачать по ссылке: https://iarduino.ru/file/538.html
//  Подробное описание функций бибилиотеки доступно по ссылке: https://wiki.iarduino.ru/page/NMEA-protocol-parser/
//  Библиотека является собственностью интернет магазина iarduino.ru и может свободно использоваться и распространяться!
//  При публикации устройств или скетчей с использованием данной библиотеки, как целиком, так и её частей,
//  в том числе и в некоммерческих целях, просим Вас опубликовать ссылку: http://iarduino.ru
//  Автор библиотеки: Панькин Павел.
//  Если у Вас возникли технические вопросы, напишите нам: shop@iarduino.ru

#ifndef iarduino_GPS_NMEA_h																											//
#define iarduino_GPS_NMEA_h																											//
																																	//
#if defined(ARDUINO) && (ARDUINO >= 100)																							//
	#include <Arduino.h>																											//
#else																																//
	#include <WProgram.h>																											//
#endif																																//
																																	//
#define GPS_TimeOUT			2000	/*	Можно изменить	*/																			//	Максимальное время отводимое на чтение данных.
#define GPS_Century			21		/*	Можно изменить	*/																			//	Текущий век. (используется для корректировки даты, если прочитаны только последние две цифры года).
#define GPS_TimeZone		3.0f	/*	Можно изменить	*/																			//	Часовой пояс по умолчанию.
																																	//
#define GPS_AutoDetectZone	13.0f																									//	Параметр функции timeZone() указывающий находить часовой пояс автоматически.
#define GPS_VISIBLE			0																										//	Номер элемента массива satellites хранящего количество наблюдаемых спутников.
#define GPS_ACTIVE			1																										//	Номер элемента массива satellites хранящего количество спутников участвующих в позиционировании.
#define GPS_StatusGGA 		0																										//	Позиция символа строки status указывающего на способ вычисления координат.
#define GPS_StatusGLL1 		2																										//	Позиция символа строки status указывающего на достоверность полученных координат.
#define GPS_StatusGLL2 		1																										//	Позиция символа строки status указывающего на способ вычисления координат.
#define GPS_StatusRMC1 		3																										//	Позиция символа строки status указывающего на достоверность полученных координат.
#define GPS_StatusRMC2 		4																										//	Позиция символа строки status указывающего на способ вычисления координат.
#define GPS_StatusRMC3		7																										//	Позиция символа строки status указывающего на статус навигации.
#define GPS_StatusVTG		5																										//	Позиция символа строки status указывающего на способ вычисления скорости и курса.
#define GPS_StatusZDA		6																										//	Позиция символа строки status указывающего на достоверность полученной даты.
																																	//
class iarduino_GPS_NMEA{																											//
	public:																															//
	/**	Пользовательские функции **/																								//
		bool		begin			(Stream &i, bool j=1		){ _objSerial=&i; _flgDataOld=j; _flgBegin=true; return true; }		//	Определяем функцию инициализации парсера				(Параметр:  объект для работы с UART, флаг указывающий выводить предыдущие данные при потере связи со спутниками или недостоверными результатами вычислений).
		float		timeZone		(float time=14.0f			){_flgAutoTimeZone=(time==GPS_AutoDetectZone?1:0); if(abs(time)<13.0f){_timeZone=time; } return _timeZone; }	//	Определяем функцию указания/получения часовой зоны		(Параметр:  ±часовой пояс или GPS_AutoDetectZone).
		template					<typename t,size_t j,size_t k>																	//	Определяем шаблон для следующей функции					(Параметр:  t-тип массива, j-количество элементов массива, k-количество элементов подмассива).
		bool		read			(t(&i)[j][k], bool f=false	){ return _read(&i[0][0], j, k, f    ); }							//	Определяем функцию чтения данных NMEA из UART			(Параметры: массив для данных о спутниках, флаг получения данных только о спутниках участвующих в позиционировании).
		template					<typename t,size_t j         >																	//	Определяем шаблон для следующей функции					(Параметр:  t-тип массива, j-количество элементов массива).
		bool		read			(t(&i)[j]   , bool f=false	){ return _read(&i[0]   , j, 1, f    ); }							//	Определяем функцию чтения данных NMEA из UART			(Параметры: массив для данных о спутниках, флаг получения данных только о спутниках участвующих в позиционировании).
		bool		read			(void						){ return _read( 0      , 0, 0, false); }							//	Определяем функцию чтения данных NMEA из UART			(Параметр:  отсутствует).
		void		timeOut			(uint32_t i					){ _setTimeOut = i; }												//  Определяем функцию задания макс. времени чтения данных	(Параметр:  максимальное время чтения данных функцией read() в миллисекундах).
	/**	Пользовательские переменные **/																								//
		uint8_t		errTim			= 0;																							//	Определяем переменную для хранения ошибки парсинга		(0 - нет ошибок, 1 - нет ответа, 2 - полученное время недостоверно).
		uint8_t		errDat			= 0;																							//	Определяем переменную для хранения ошибки парсинга		(0 - нет ошибок, 1 - нет ответа, 2 - полученная дата  недостоверна).
		uint8_t		errPos			= 0;																							//	Определяем переменную для хранения ошибки парсинга		(0 - нет ошибок, 1 - нет ответа, 2 - не определён способ вычисления координат, или полученные координаты недостоверны).
		uint8_t		errCrs			= 0;																							//	Определяем переменную для хранения ошибки парсинга		(0 - нет ошибок, 1 - нет ответа, 2 - не определён способ вычисления скорости и курса).
		char		status[9]		= "********";																					//	Определяем строку     для хранения символов статуса		(способ вычисления координат, достоверность полученных координат, способ вычисления координат, достоверность полученных координат, способ вычисления координат, статус навигации, способ вычисления скорости и курса, достоверность полученной даты).
		uint16_t	available		= 0;																							//	Определяем переменную для хранения флагов чтения		(переменная состоит из битов указывающих о чтении индентификаторов: 0-GGA, 1-GLL, 2-RMC, 3-VTG, 4-ZDA, 5-DHV, 6-GST, 7-GSA1, 8-GSA2, 9-GSA3, 10-GSV, 11-TXT, 15-флаг устанавливается если один из учитываемых идентификаторов прочитан дважды значит прочитано всё сообщение NMEA).
		float		latitude		= 0.0f;																							//	Определяем переменную для хранения широты				(± 90.0) N-северная  «+», S-южная    «-».
		float		longitude		= 0.0f;																							//	Определяем переменную для хранения долготы				(±180.0) E-восточная «+», W-западная «-».
		uint16_t	altitude		= 0;																							//	Определяем переменную для хранения высоты				(±32767) Над уровнем моря.
		uint8_t		satellites[2]	= {0,0};																						//	Определяем массив     для хранения количества спутников	(0-255,0-255) наблюдаемые, участвующие в позиционировании.
		uint8_t		speed			= 0;																							//	Определяем переменную для хранения скорости				(0-255)  км/ч.
		float		course			= 0.0f;																							//	Определяем переменную для хранения курса				(±180.0°).
		float		PDOP			= 25.50f;																						//	Определяем переменную для хранения PDOP					(пространственный геометрический фактор ухудшения точности: 0-1 идеальная точность, 2-3 отличная точность, 4-6 хорошая точность, 7-8 средняя точность, 9-20 точность ниже среднего, 21-25.5 плохая точность).
		float		HDOP			= 25.50f;																						//	Определяем переменную для хранения HDOP					(горизонтальный   геометрический фактор ухудшения точности: 0-1 идеальная точность, 2-3 отличная точность, 4-6 хорошая точность, 7-8 средняя точность, 9-20 точность ниже среднего, 21-25.5 плохая точность).
		float		VDOP			= 25.50f;																						//	Определяем переменную для хранения VDOP					(вертикальный     геометрический фактор ухудшения точности: 0-1 идеальная точность, 2-3 отличная точность, 4-6 хорошая точность, 7-8 средняя точность, 9-20 точность ниже среднего, 21-25.5 плохая точность).
		uint8_t		seconds			= 0;																							//	Определяем переменную для хранения секунд				(0-59).
		uint8_t		minutes			= 0;																							//	Определяем переменную для хранения минут				(0-59).
		uint8_t		hours			= 0;																							//	Определяем переменную для хранения часов				(1-12).
		uint8_t		Hours			= 0;																							//	Определяем переменную для хранения часов				(0-23).
		uint8_t		midday			= 0;																							//	Определяем переменную для хранения полденя				(0-am, 1-pm).
		uint8_t		day				= 0;																							//	Определяем переменную для хранения дня месяца			(1-31).
		uint8_t		weekday			= 0;																							//	Определяем переменную для хранения дня недели			(0-воскресенье, 1-понедельник, ... , 6-суббота).
		uint8_t		month			= 0;																							//	Определяем переменную для хранения месяца				(1-12).
		uint8_t		year			= 0;																							//	Определяем переменную для хранения года					(0-99 без учёта века).
		uint16_t	Year			= 0;																							//	Определяем переменную для хранения года					(0-65535 с учётом века).
		uint32_t	Unix			= 0;																							//	Определяем переменную для хранения Unix времени			(секунды прошедшие с начала эпохи Unix 01.01.1970 00:00:00 GMT).
	private:																														//
	/**	Внутренние переменные **/																									//
		Stream*		_objSerial;																										//	Объявляем  указатель на объект работы с UART			(Serial, Serial1, ..., SoftwareSerial).
		bool		_flgBegin		= false;																						//	Определяем флаг инициализации.
		uint32_t	_setTimeOut		= GPS_TimeOUT;																					//	Определяем переменную для хранения макс. времени чтения	(максимальное время чтения данных функцией read).
		float		_timeZone		= GPS_TimeZone;																					//	Определяем временную зону								(±12.0 часов).
		uint8_t		_flgAutoTimeZone= 0;																							//	Определяем флаг указывающий автоопределение врем. зоны	(0-нет, 1-автоопределение).
		bool		_flgDataOld;																									//	Объявляем  флаг указывающих выводить старые данные		(если нет новых).
	/**	Внутренние функции **/																										//
		bool		_read			(uint8_t*,uint8_t,uint8_t,bool	);																//	Объявляем  функцию чтения пакета NMEA					(Параметры: указатель на начало массива данных о спутниках, количество спутников, количество данных о спутнике, флаг получения данных только о спутниках участвующих в позиционировании).
		uint8_t		_findIndexID	(uint8_t,uint8_t*,uint8_t,uint8_t);																//	Объявляем  функцию обнаружения ID спутника в массиве	(Параметры: искомый ID, указатель на начало массива данных о спутниках, количество спутников, количество данных о спутнике).
		void		_parseDate		(float num){uint32_t i=num; year   =i%100; i/=100; month  =i%100; i/=100; day  =i%100; if(status[GPS_StatusZDA]=='*'){status[GPS_StatusZDA]='V';} if(status[GPS_StatusZDA]=='V' && num){status[GPS_StatusZDA]='D';} if(status[GPS_StatusZDA]=='T' && num){status[GPS_StatusZDA]='A';} }	//	Парсер даты		(Параметр:  отсутствует). Функция парсит время в переменные day, month, year.
		void		_parseTime		(float num){uint32_t i=num; seconds=i%100; i/=100; minutes=i%100; i/=100; Hours=i%100; if(status[GPS_StatusZDA]=='*'){status[GPS_StatusZDA]='V';} if(status[GPS_StatusZDA]=='V' && num){status[GPS_StatusZDA]='T';} if(status[GPS_StatusZDA]=='D' && num){status[GPS_StatusZDA]='A';} }	//	Парсер времени	(Параметр:  отсутствует). Функция парсит время в переменные Hours, minutes, seconds.
		float		_parsePos		(float pos, float dir){float i=(uint32_t)pos/100; i+=(pos-i*100)/60.0f; if(((uint8_t)dir+'0')=='S'||((uint8_t)dir+'0')=='W'){i*=-1.0f;} return i;}	//	Парсер координат	(Параметры: координаты, курс).
		void		_UnixToTime		(void							);																//	Объявляем  функцию получения времени из секунд Unix		(Параметр:  отсутствует).
		void		_TimeToUnix		(void							);																//	Объявляем  функцию получения секунд Unix из времени		(Параметр:  отсутствует).
};																																	//
#endif																																//
