#ifndef PARSER_FUNC_H
#define PARSER_FUNC_H

#include "ParserType.h"
#include "ErrorType.h"
#include "TypesOfType.h"


// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------
Status_t P_ParseGlobal (Node_t* start_node, size_t amount,
                        Node_t* global_node, const char* name_file,
                        StringTable_t* table_str);
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseUnion (ParserInf_t* inf,
                       Node_t* node);

Status_t P_ParseInOut (ParserInf_t* inf,
                       Node_t* parent);

Status_t P_ParseAssign (ParserInf_t* inf,
                        Node_t* parent);

Status_t P_ParseWhile (ParserInf_t* inf,
                       Node_t* parent);

Status_t P_ParseBreak (ParserInf_t* inf,
                       Node_t* parent);

Status_t P_ParseContinue (ParserInf_t* inf,
                          Node_t* parent);

Status_t P_ParseConditional (ParserInf_t* inf,
                             Node_t* parent);

Status_t P_ParseReturn (ParserInf_t* inf,
                        Node_t* parent);

Status_t P_ParseDeclarationFunc (ParserInf_t* inf,
                                 Node_t* parent);

Status_t P_ParseDeclarationVar (ParserInf_t* inf,
                                Node_t* parent);

Status_t P_ParseDeclarationStruct (ParserInf_t* inf,
                                   Node_t* parent);

TreeType_t P_ParseType (ParserInf_t* inf);

Status_t P_ParseOrExpression (ParserInf_t* inf,
                              Node_t* parent);

Status_t P_ParseAndExpression (ParserInf_t* inf,
                               Node_t* parent);

Status_t P_ParseEqualExpression (ParserInf_t* inf,
                                 Node_t* parent);

Status_t P_ParseCompareExpression (ParserInf_t* inf,
                                   Node_t* parent);

Status_t P_ParseAddSub (ParserInf_t* inf,
                        Node_t* parent);

Status_t P_ParseMulDiv (ParserInf_t* inf,
                        Node_t* parent);

Status_t P_ParseUnaryExpression (ParserInf_t* inf,
                                 Node_t* parent);

Status_t P_ParsePrimaryExpression (ParserInf_t* inf,
                                   Node_t* parent);

Status_t P_ParseCallFunction (ParserInf_t* inf,
                              Node_t* parent);

Status_t P_ParseVariable (ParserInf_t* inf,
                          Node_t* parent);

// ---------------------------------------------------------------------------------------------------
#define isComma(__node__)       checkPunctNode (__node__, PUNCT_COMMA)

#define isLeftRound(__node__)   checkPunctNode (__node__, PUNCT_LPAREN)
#define isRightRound(__node__)  checkPunctNode (__node__, PUNCT_RPAREN)

#define isLeftQuad(__node__)  checkPunctNode (__node__, PUNCT_LBRACKET)
#define isRightQuad(__node__)  checkPunctNode (__node__, PUNCT_RBRACKET)

#define isLeftTang(__node__)    checkPunctNode (__node__, PUNCT_LTANG)
#define isRightTang(__node__)   checkPunctNode (__node__, PUNCT_RTANG)

#define isArrow(__node__) checkPunctNode (__node__, PUNCT_ARROW)

#define isEndChar(__node__)     checkPunctNode (__node__, PUNCT_ENDSTRING)
// ---------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция проверки узла на соответствие с типом пунктуации и значением
 * \param [in] node Узел, который проверяют
 * \param [in] val ЗначениеЮ которое должно быть у узла
 * \return 1 - если совпадает, иначе 0
 */
int checkPunctNode (Node_t* node, int val);
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция смены текущего узла в контексте
 @param [in] inf Указатель на структуру контекста
*/
int nextNode (ParserContextInf_t* inf);
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция пропуска всех разделяющий символов (\n и \)
 * \param [in] inf Указатель на структуру контекста
*/
int skipVoid (ParserContextInf_t* inf);
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция выдачи следующего неконечного контекстного токена
 @param [in] inf Указатель на структуру контекста
 @return Узел
*/
Node_t* getNextNotEndNode (ParserContextInf_t* inf);
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция вывода ошибки по её номеру
 @param [in] error Номер ошибки
 @param [in] inf Указатель на структуру контекста
*/
int parserError (ParserError_t error,
                 ParserContextInf_t* inf);
// ---------------------------------------------------------------------------------------------------


#endif /* PARSER_FUNC_H */
