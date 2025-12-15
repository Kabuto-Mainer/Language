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
    PE_INDENT_NO_END,
    PE_ASSIGN,
    PE_CALL_FUNC,
    PE_NOT_CONDITION,

    PE_NO_END_CHAR,
    PE_NOT_RIGHT_ROUND,
    PE_NOT_LEFT_ROUND,
    PE_NOT_LEFT_TANG,
    PE_NOT_RIGHT_TANG,

    PE_NOT_UNION,
    PE_NOT_EXPRESION,
    PE_NOT_INDENT,
    PE_NOT_RETURN_VALUE,
    PE_NOT_TYPE
};
// ---------------------------------------------------------------------------------------------------


#endif /* ERROR_TYPE_H */
