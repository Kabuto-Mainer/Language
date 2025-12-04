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
    PE_NOT_INDENT_IN_FUNC,
    PE_NOT_RIGHT_ROUND,
    PE_NOT_LEFT_ROUND,
    PE_UNKNOWN_VALUE,
    PE_NOT_EXPRESION_AFTER_OPER,
    PE_NOT_EXPRESION_IN_FUNC,
    PE_NOT_INDENT_AFTER_DECLARATION,
    PE_NOT_EXPRESION_IN_ASSIGN,
    PE_NOT_EXPRESION,
    PE_NOT_UNION,
    PE_NOT_END_CHAR
};
// ---------------------------------------------------------------------------------------------------


#endif /* ERROR_TYPE_H */
