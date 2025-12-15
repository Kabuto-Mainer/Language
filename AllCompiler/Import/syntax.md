# Примерный Синтаксис

## Типы данных

| Тип | Синтаксис | Описание |
|-----|-----------|----------|
| **int** | `int` | 32-битное целое |
| **pointer** | `&int` | Указатель на int (`&` + базовый тип) |
| **string** | `str` | Массив ASCII + `\0` (вывод посимвольно) |

## Переменные

al int num = 0
al int num
al & int ptr = & num
al & int ptr

## Массивы (только int, одномерные)

al int mass<10> = @5, 4, 2, 7, 3, 1, 5, 6@
mass<4> = 45


## Структуры

### Вариант 1
block Point <
    int x
    &int y
    str name<10>
>

### Вариант 2
block Point < int x ~ &int y ~ str name<10> >


## Функции

ser Name <int arg, &int ptr, str name<10> >     -> int (&int, str<10>)
(
```Тело функции```
)


## Ключевые слова

| Слово | Назначение | Пример |
|-------|------------|---------|
| `al` | alloc (объявление переменной) | `al int x` |
| `ser` | service (функция) | `ser fact <i32 n>` |
| `tr` | if | `tr <x > 0>` |
| `ntr` | else if | `ntr <x < 10>` |
| `def` | else | `def (...)` |
| `ret` | return | `ret 0` |
| `loop` | while | `loop <i < 10>` |
| `=` | assign | `x = 5` |
| `err` | break | `err` |
| `in` | input | `in <x>` |
| `out` | output | `out <x>` |

## Блоки кода

( Открытие блока кода
Сам блок кода
)
