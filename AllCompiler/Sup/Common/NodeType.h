#ifndef NODE_TYPE_H
#define NODE_TYPE_H

#include <stdio.h>
#include "TypeTable.h"
#include "StringTable.h"
#include "SymbolTable.h"

// ---------------------------------------------------------------------------------------------------
struct Node_t;
// ---------------------------------------------------------------------------------------------------
// Во время парсинга нам нужно только имя
struct ID_Raw_t
{
    StringEntry_t* name; // Только имя из StringTable
};

// ---------------------------------------------------------------------------------------------------
// Используется при построении дерева при объявлении переменных и функций
struct ID_Tree_t
{
    StringEntry_t* name;
    TypeEntry_t* base_type;   // Тип (в котором есть и количество элементов, и уровень вложенности)
};

// ---------------------------------------------------------------------------------------------------
// Появляется на промежуточном этапе проверки всех типов и заполнения таблиц имен
struct ID_Resolved_t
{
    SymbolEntry_t* symbol; // Указатель на элемент таблицы имен
};

// ---------------------------------------------------------------------------------------------------
// Тип результата операции
struct OperatorType_t
{
    TypeEntry_t* result_type;
};

// ---------------------------------------------------------------------------------------------------
// Сам union
union NodeValue_t
{
    // Обычные значения для простых узлов
    int key;
    int punct;
    int num;

    // Идентификаторы на разных этапах
    struct ID_Raw_t id_raw;
    struct ID_Tree_t id_tree;
    struct ID_Resolved_t id_res;

    // Операторы и выражения
    struct {
        int code;
        TypeEntry_t* type;          // Тип результата операции
    } oper;

    // Блоки кода
    struct {
        Node_t* prev;              // Ссылка на предыдущий блок кода для организации областей видимости
        SymbolTable_t* scope;      // Таблица символов этого блока
    } block;

    // Функции
    struct {
        StringEntry_t* name;       // Имя функции
        TypeEntry_t* return_type;  // Тип возвращаемого значения
        SymbolTable_t* params;     // Таблица аргументов
    } func;

    // Структуры и типы
    struct {
        StringEntry_t* name;       // Имя структуры
        TypeEntry_t* type_entry;   // Ссылка на запись в таблице типов
    } struct_def;
};

// ---------------------------------------------------------------------------------------------------
/// @brief Типы узлов дерева
enum NodeType_t
{
    NODE_NUM,
    NODE_ID_RAW,
    NODE_ID_TREE,
    NODE_ID_RESOLVED,

    NODE_OPER,
    NODE_KEY, // Только для токенизации
    NODE_CALL,

    NODE_IF,
    NODE_WHILE,
    NODE_RETURN,
    NODE_BREAK,
    NODE_CONTINUE,

    NODE_VAR_DECL,
    NODE_FUNC_DECL,
    NODE_STRUCT_DECL,

    NODE_BLOCK,
    NODE_PROGRAM,

    NODE_PUNCT
};

// ---------------------------------------------------------------------------------------------------
/// @brief Основная структура узла дерева
struct Node_t
{
    NodeType_t type_node;

    struct Node_t* parent;
    struct Node_t** children;
    int amount_children;

    NodeValue_t val;
};

// ---------------------------------------------------------------------------------------------------

#endif /* NODE_TYPE_H */
