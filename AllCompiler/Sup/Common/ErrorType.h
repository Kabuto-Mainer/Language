#ifndef ERROR_TYPE_H
#define ERROR_TYPE_H

// ---------------------------------------------------------------------------------------------------
/// @brief Возможные статусы при лексическом и синтаксическом анализе
enum Status_t
{
    PARSER_NOT_THIS     = 0,
    PARSER_THIS_OK      = 1,
    PARSER_SYNTAX_ERROR = 2
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
enum ParserError_t
{
    PE_NOT_ERROR,
    PE_UNKNOWN,
    PE_ERROR,

    PE_NO_ASSIGN,
    PE_NO_CONDITION,
    PE_NO_UNION,
    PE_NO_PARAM,
    PE_NO_COLON,
    PE_NO_TYPE,
    PE_NO_EXPRESSION,

    PE_INCORRECT_NAME,
    PE_INCORRECT_TYPE,
    PE_INCORRECT_ARRAY_SIZE,

    PE_NO_END_CHAR,
    PE_NOT_RIGHT_ROUND,
    PE_NOT_LEFT_ROUND,
    PE_NOT_RIGHT_TANG,
    PE_NOT_LEFT_TANG,
    PE_NOT_RIGHT_BRACKET,
    PE_NOT_LEFT_BRACKET,
};
// ---------------------------------------------------------------------------------------------------


#endif /* ERROR_TYPE_H */
