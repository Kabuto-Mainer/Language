#ifndef NODE_TYPE_H
#define NODE_TYPE_H


// ---------------------------------------------------------------------------------------------------
/// @brief Тип узла
enum NodeType_t
{
    NODE_TYPE_INDENT,
    NODE_TYPE_FUNC,
    NODE_TYPE_VAR,
    NODE_TYPE_OPER,
    NODE_TYPE_KEY_WORD,
    NODE_TYPE_PUNCT,
    NODE_TYPE_NUM,
    NODE_TYPE_BLOCK
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/// @brief Тип операции
enum OperType_t
{
    OPER_ADD                = 0,
    OPER_SUB                = 1,
    OPER_MUL                = 2,
    OPER_DIV                = 3,
    OPER_POW                = 4,

    OPER_COMP_BIG_EQUAL     = 5,
    OPER_COMP_ONLY_BIG      = 6,
    OPER_COMP_LIT_EQUAL     = 7,
    OPER_COMP_ONLY_LIT      = 8,
    OPER_COMP_EQUAL         = 9,
    OPER_COMP_NOT_EQUAL     = 10,
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/// @brief Тип ключевого слова
enum KeyWord_t
{
    KEY_IF              = 0,
    KEY_ELIF            = 1,
    KEY_ELSE            = 2,
    KEY_WHILE           = 3,
    KEY_EXTERN_VAR      = 4,
    KEY_EXTERN_FUNC     = 5,
    KEY_RETURN          = 6,
    KEY_ASSIGN          = 7
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/// @brief Тип пунктуационного символа
enum Punct_t
{
    PUNCT_COMMA         = 0,
    PUNCT_LEFT_ROUND    = 1,
    PUNCT_RIGHT_ROUND   = 2,
    PUNCT_DOG           = 3,
    PUNCT_H             = 4,
    PUNCT_QUOT          = 5,
    PUNCT_END_STR       = 6
};
// ---------------------------------------------------------------------------------------------------


#endif /* NODE_TYPE_H */
