#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "SupportFunc.h"

// ---------------------------------------------------------------------------------------------------
/// @brief Адрес log файла
const char* ADDRESS_LOG_FILE = "Dump/dump.log";
// ---------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------
/**
 * @brief Хеш-функция
 * @param [in] string Строка, которая преобразуется в хеш
 * @return Хеш строки
*/
size_t getHash (const char* string)
{
    assert (string);

    size_t index = 0;
    size_t hash = 5381;
    while (string[index] != '\0')
    {
        hash = (hash * 33 + (size_t) string[index]);
        index++;
    }

    return hash;
}
// --------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
/**
 * @brief Функция, находящая размер файла в байтах по его имени
 * @param [in] name_file Имя файла
 * @return Размер файла
*/
size_t getFileSize (const char* name_file)
{
    assert(name_file);

    struct stat file_stat = {};

    if (stat (name_file, &file_stat) == -1)
    {
        printf ("ERROR with stat\n");
        return 0;
    }
//     Вроде как размер не должен быть отрицательным
    return (size_t) file_stat.st_size;
}
// -------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция загрузки содержимого файла в буфер
 @param [in] name_file Имя загружаемого файла
 @return Указатель на выделенную динамически память
*/
char* createCharBuffer (const char* name_file)
{
    assert (name_file);

    size_t size = getFileSize (name_file);
    char* buffer  = (char*) calloc (size + 1, sizeof (char));
    if (buffer == NULL)
        EXIT_FUNC("NULL calloc", NULL);

    FILE* stream = fopen (name_file, "r");
    if (stream == NULL)
        EXIT_FUNC("NULL file", NULL);

    size_t amount_checks = fread (buffer, sizeof (char), size, stream);
    fclose (stream);
    buffer[amount_checks] = '\0';

    return buffer;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция отправки информации в лог файл
 @param [in] text Текст сообщения
 @param [in] file Файл откуда произошел вызов
 @param [in] line Строка откуда произошел вызов
*/
int pushLogFile (const char* text,
                 const char* file,
                 int line)
{
    assert (text);

    FILE* stream = fopen (ADDRESS_LOG_FILE, "a");
    if (stream == NULL)
    {
        printf ("ERROR (%s:%d): Can not open log file\n", __FILE__, __LINE__);
        return 1;
    }

    fprintf (stream, "[%ld][%15s:3%d]: %s\n",
             clock (), file, line, text);
    // fprintf (stream, "%s\n", text);
    // fprintf (stream, "[%s:%d]\nTime start = %ld\nTime call = %ld\nTime work = %ld\n",
    //         file, line, TIME_START, clock (), clock () - TIME_START);

    fclose (stream);

    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция очистки log файла
*/
int cleanLogFile ()
{
    FILE* stream = fopen (ADDRESS_LOG_FILE, "w");
    if (stream == NULL)
    {
        Log_Error ("NULL file");
        return 1;
    }

    fprintf (stream, "Log file of program diff\n");
    fclose (stream);

    return 0;
}
// ---------------------------------------------------------------------------------------------------




