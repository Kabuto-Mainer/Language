#ifndef CONST_H
#define CONST_H

#define OFF 0
#define ON 1

#define PROCESSOR 0
#define DEBUG 1


#define MOD_WORK DEBUG

// Канареечная зашита
#define BIRD_SECURE OFF

// Хеш зашиты
#define HASH_SECURE OFF

//todo config.h
// Автоматическое увеличение стека
#define AUTO_REALLOC ON

// Заполнение стека
//* Необходимо для работы Канареечной и Хеш зашиты
#define COMPLETION_DATA OFF


// Тип данных стека. В режиме процессора должен быть <int>
typedef int stmn_t;
const char name_type[] = "int";
#define PRINT_ELEMENT(num) { \
   printf("%d", num); \
}


// Количество регистров
const int AMOUNT_REGISTERS = 12;

// Количество меток и переменных
const int AMOUNT_POINTS = 20;
const int AMOUNT_VARS = 20;

// Множитель realloc
const int MOD_REALLOC = 2;
const int REALLOC_CONST = 5;

// Начальное количество команд
const size_t START_AMOUNT_CMD = 1e5;

const int START_SIZE_STACK = 4;

// Минимальное и максимальное значение аргументов
const stmn_t MAX_MEAN = 2 * 1e8;
const stmn_t MIN_MEAN = -2 * 1e8;

// Сид для хеш-функции
const size_t START_HASH_CODE = 5318;

// Ядовитое значение для стека
const stmn_t POISON_NUM = -3333;

// Минимальные рабочий адрес
const int MIN_ADDRESS = 8000;

// Количество элементов, которое выведется, если size > этого значения
const int AMOUNT_PRINT_ELEMENT = 10;

// Куропатка или Канарейка
const stmn_t BIRD_NUM = (stmn_t) 0xDEADFACE;

// Размер выделяемой оперативки
const int RAM_SIZE = 100 * sizeof(int);

// Константа уменьшения времени
const long long int TIME_DIV = 1e7;

// Количество доп. элементов в bin-code
const int AMOUNT_SUP_NUM = 7;

// Сингнатура и текущая версия
const int OWN_SIGNATURE = 1388;
const int CURRENT_VERSION = 3;



// Размер одного пикселя
const int PIXEL_SIZE = 1;

// Стандартные настройки графики
const int LEN_DISPLAY = 160;
const int HIGH_DISPLAY = 120;
const int VRAM_SIZE = LEN_DISPLAY * HIGH_DISPLAY;

const int U_AUDIO = 44100;
const int SUPPORT_FPS = 10;
const int CHANAL = 1;

const int LATE_SOUND_FRAME = 2;


const char NAME_SOUND_STREAM[][100] =
{
   "PROCESSOR/all-sound/stream0.raw",
   "PROCESSOR/all-sound/stream1.raw",
   "PROCESSOR/all-sound/stream2.raw",
   "PROCESSOR/all-sound/stream3.raw",
   "PROCESSOR/all-sound/stream4.raw",
   "PROCESSOR/all-sound/stream5.raw",
   "PROCESSOR/all-sound/stream6.raw",
   "PROCESSOR/all-sound/stream7.raw",
   "PROCESSOR/all-sound/stream8.raw"
};

const int AMOUNT_SOUND_STREAM = sizeof(NAME_SOUND_STREAM) / sizeof(NAME_SOUND_STREAM[0]);

#endif // CONST_H
