#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "TokenFunc.h"
#include "../Vector/VectorFunc.h"
#include "../Support/SupportFunc.h"


// ---------------------------------------------------------------------------------------------------
int skipVoid (char** str)
{
    assert (str);
    assert (*str);

    while (**str == ' ' || **str == '\t' || **str == '\r')
        ++ *str;
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int tokenGlobal (char* buffer,
                 TokenVector_t* vector)
{
    assert (buffer);
    assert (vector);

    char** str = &buffer;
    ParserPlaceInf_t inf = {
        .pose = str};

    while (**str != '$' && **str != '\0')
    {
        skipVoid (str);
        if (tokenPunct (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenMathOper (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenNum (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenProgSyntax (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenIndent (&inf, vector) == PARSER_THIS_OK)
            continue;

        EXIT_FUNC ("UNKNOWN SYNTAX", 1);
    }
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
ParserStatus_t tokenPunct (ParserPlaceInf_t* inf,
                           TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    // printf ("Punct\n");
    Token_t token = {
        .start = *inf->pose,
        .len_name = 1};

    switch (**inf->pose)
    {
        case ('('):    { token.type = TOKEN_LEFT_ROUND; break; }
        case (')'):    { token.type = TOKEN_RIGHT_ROUND; break; }
        case ('@'):    { token.type = TOKEN_DOG; break; }
        case (','):    { token.type = TOKEN_COMMA; break; }
        case ('\n'):   { token.type = TOKEN_END_STR; break; }
        default: return PARSER_NOT_THIS;
    }
    ++ *inf->pose;
    vectorAdd (vector, token);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
ParserStatus_t tokenNum (ParserPlaceInf_t* inf,
                         TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    // printf ("Num\n");
    if (!isdigit (**inf->pose) || (**inf->pose == '-' && !isdigit (*(*inf->pose + 1))))
        return PARSER_NOT_THIS;

    double value = 0;
    int len = 0;
    sscanf (*(inf->pose), "%lf%n", &value, &len);

    Token_t token = {
        .type = TOKEN_NUMBER,
        .start = *(inf->pose),
        .len_name = (size_t) len};
    vectorAdd (vector, token);

    *(inf->pose) += len;
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
ParserStatus_t tokenIndent (ParserPlaceInf_t* inf,
                            TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    // printf ("Indent\n");
    if (!isalpha (**inf->pose))
        return PARSER_NOT_THIS;

    size_t len = 0;
    char** str = inf->pose;
    while (isalpha ((*str)[len]) || isdigit ((*str)[len]) || (*str)[len] == '_')
        len++;

    Token_t token = {
        .type = TOKEN_INDENT,
        .start = *str,
        .len_name = len};
    vectorAdd (vector, token);

    *str += len;
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
ParserStatus_t tokenProgSyntax (ParserPlaceInf_t* inf,
                                TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    // printf ("ProgSyntax\n");
    size_t len = 0;
    char** str = inf->pose;
    while (isalpha((*str)[len]))
    {
        len++;
    }

    char buffer_char = (*str)[len];
    (*str)[len] = '\0';

    Token_t token = {
        .start = *str};

    if      (strcmp ("alloc", *str) == 0)   token.type = TOKEN_EXTERN;
    else if (strcmp ("trig", *str) == 0)    token.type = TOKEN_IF;
    else if (strcmp ("nexttrig", *str) == 0)token.type = TOKEN_ELIF;
    else if (strcmp ("default", *str) == 0) token.type = TOKEN_ELSE;
    else if (strcmp ("loop", *str) == 0)    token.type = TOKEN_WHILE;
    else
    {
        *str[len] = buffer_char;
        return PARSER_NOT_THIS;
    }

    token.len_name = len;
    (*str)[len] = buffer_char;
    *str += len;
    vectorAdd (vector, token);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
ParserStatus_t tokenMathOper (ParserPlaceInf_t* inf,
                              TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    // printf ("Math\n");
    size_t len = 0;
    char** str = inf->pose;
    while (ispunct((*str)[len]))
        len++;

    char buffer_char = (*str)[len];
    (*str)[len] = '\0';

    Token_t token = {
        .start = *str,
        .len_name = 2};

    if      (strcmp ("**", *str) == 0)  token.type = TOKEN_POW;
    else if (strcmp ("<=", *str) == 0)  token.type = TOKEN_LIT_EQUAL;
    else if (strcmp (">=", *str) == 0)  token.type = TOKEN_BIG_EQUAL;
    else if (strcmp ("==", *str) == 0)  token.type = TOKEN_EQUAL;
    else if (strcmp ("!=", *str) == 0)  token.type = TOKEN_NOT_EQUAL;
    else if (len == 1)
    {
        token.len_name = 1;
        switch (**str)
        {
            case ('+'):    { token.type = TOKEN_ADD; break; }
            case ('-'):    { token.type = TOKEN_SUB; break; }
            case ('*'):    { token.type = TOKEN_MUL; break; }
            case ('/'):    { token.type = TOKEN_DIV; break; }
            case ('<'):    { token.type = TOKEN_ONLY_LIT; break; }
            case ('>'):    { token.type = TOKEN_ONLY_BIG; break; }
            default:
            {
                (*str)[len] = buffer_char;
                return PARSER_NOT_THIS;
            }
        }
    }
    else
    {
        (*str)[len] = buffer_char;
        return PARSER_NOT_THIS;
    }

    (*str)[len] = buffer_char;
    *str += token.len_name;
    vectorAdd (vector, token);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int tokenDump (TokenVector_t* vector)
{
    assert (vector);

    for (size_t i = 0; i < vectorGetSize (vector); i++)
    {
        Token_t* token = vectorGetToken (vector, i);
        printf ("[%zu]<%.*s>[%d]\n", i, (int) token->len_name, token->start, token->type);
    }
    return 0;
}
// ---------------------------------------------------------------------------------------------------



//
// // ---------------------------------------------------------------------------------------------------
// static size_t AMOUNT_BLOCKS = 0;
// // ---------------------------------------------------------------------------------------------------
// struct ReplacePlaceInf_t
// {
//     char** old_str;
//     char** new_str;
//
// };
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// ParserStatus_t replaceGlobal (ReplacePlaceInf_t* inf)
// {
// }
//
//
// ParserStatus_t replaceVar (ReplacePlaceInf_t* inf,
//                            CompilerBlock_t* block)
// {
//     assert (inf);
//     assert (block);
//
//     char** s_old = inf->old_str;
//     char** s_new = inf->new_str;
//
//     if (!isdigit (**s_old))
//         return PARSER_NOT_THIS;
//
//     char* name = NULL;
//     int len_name = 0;
//     sscanf (*s_old, "%[a-zA-Z0-9_]%n", name, &len_name);
//
//     size_t number_block = 0;
//     size_t index_in_table = 0;
//     bool is_really = false;
//
//     while (block != NULL)
//     {
//         if (block->table_var != NULL)
//         {
//             index_in_table = nameTableFind (block->table_var, name);
//             if (index_in_table != (size_t) -1)
//             {
//                 number_block = block->number;
//                 is_really = true;
//                 break;
//             }
//         }
//         block = block->parent;
//     }
//     *sprintf (*s_new, " V:%zu:%zu ", number_block, index_in_table);
//     *s_old += len_name;
//     skipVoid (s_old);
//
//     return PARSER_THIS_OK;
// }
//
// ParserStatus_t replaceFunc (ReplacePlaceInf_t* inf,
//                             CompilerBlock_t* block,
//                             NameTable_t* table_func)
// {
//     assert (inf);
//     assert (block);
//     assert (table_func);
//
//     char** s_old = inf->old_str;
//     char** s_new = inf->new_str;
//
//     if (!isalpha (**s_old))
//         return PARSER_NOT_THIS;
//
//     char* name = NULL;
//     int len_name = 0;
//     sscanf (*s_old, "%[a-zA-Z0-9_]%n", name, &len_name);
//
//     size_t index_in_table = 0;
//     bool is_really = false;
//
//     index_in_table = nameTableFind (table_func, name);
//     if (index_in_table == (size_t) -1)
//         return PARSER_NOT_THIS;
//
//     *s_old += len_name;
//     skipVoid (s_old);
//     if (**s_old != '(')
//         return PARSER_THIS_ERROR;
//
//     sprintf (*s_new, "F:%zu(", index_in_table);
//
//     while (block != NULL)
//     {
//         if (block->table_var != NULL)
//         {
//             index_in_table = nameTableFind (block->table_var, name);
//             if (index_in_table != (size_t) -1)
//             {
//                 number_block = block->number;
//                 is_really = true;
//                 break;
//             }
//         }
//         block = block->parent;
//     }
//
// }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// // // ---------------------------------------------------------------------------------------------------
// // CompilerNode_t* parserGlobal (ParserPlaceInf_t* inf)
// // {
// //     assert (inf);
// //
// //     CompilerNode_t* root = newNode ();
// //     CompilerBlock_t* global_block = newBlock ();
// //
// //     NameTable_t table_var = {};
// //     NameTable_t table_func = {};
// //     nameTableCtr (&table_var);
// //     nameTableCtr (&table_func);
// //     global_block->table_var = &table_var;
// //     global_block->type = BLOCK_TYPE_GLOBAL;
// //     /* scanf input data i skip */
// //     while (**(inf->pose) != '$')
// //     {
// //         if (parserOper (root, inf, table_all_func) == 0)
// //         {
// //         }
// //         else if (parserOper (root, inf, table_all_func, table
// //
// //
// //
// //     }
// // }
// // // ---------------------------------------------------------------------------------------------------
// //
// // // ---------------------------------------------------------------------------------------------------
// // int parserOper (ParserPlaceInf_t* inf,
// //                 CompilerNode_t* parent)
// // {
// //     assert (inf);
// //     assert (parent);
// //
// //     char* pose = *(inf->pose);
// //     int index = 0;
// //
// //     while (isalpha (pose[index]))
// //         index++;
// //
// //     if (
// //
// //
// // }
//
// ParserOperStatus_t parserIfOper (ParserPlaceInf_t* inf,
//                                  CompilerNode_t* parent,
//                                  CompilerBlock_t* parent_block,
//                                  NameTable_t* table_func)
// {
//     assert (inf);
//     assert (parent);
//     assert (parent_block);
//     assert (table_func);
//
//     char** pose = inf->pose;
//     if (strncmp (*pose, "if ", 3 /* len ("if ") */) != 0)
//         return PARSER_NOT_THIS;
//
//     *pose += 2;
//     skipVoid (pose);
//
//     CompilerNode_t* oper = newNode ();
//     addNode (parent, oper);
//     oper->type = NODE_TYPE_OPER;
//     oper->value.type = COMPILER_OPER_IF_ELSE;
//
//     CompilerBlock_t* block_if = newBlock ();
//     addBlock (parent_block, block_if);
//     block_if->type = BLOCK_TYPE_IF;
//
//     if (**pose != '(')
//         SYNTAX_ERROR (inf, PARSER_ABSENCE_BRACKET, PARSER_THIS_ERROR);
//
//     ++ *pose;
//     skipvoid (pose);
//
//     if (parserExpresion (inf, oper, parent_block) != PARSER_THIS_OK)
//         SYNTAX_ERROR (inf, PARSER_ABSENCE_EXPRESION, PARSER_THIS_ERROR);
//
//     CompilerNode_t* node_if = newNode ();
//     node_if->type = NODE_TYPE_OPER;
//     node_if->value.type = COMPILER_OPER_IF;
//     addNode (oper, node_if);
//
//     if (**pose != ')')
//         SYNTAX_ERROR (inf, PARSER_ABSENCE_BRACKET, PARSER_THIS_ERROR);
//     ++ *pose;
//     skipVoid (pose);
//
//     if (parserUnion (inf, node_if, block_if) != PARSER_THIS_OK)
//         SYNTAX_ERROR (inf, PARSER_ABSENCE_UNION, PARSER_THIS_ERROR);
//
// //     /* Parser else if I skip */
// //     CompilerNode_t* node_else = newNode ();
// //     node_else->type = NODE_TYPE_OPER;
// //     node_else->value.type = COMPILER_OPER_ELSE;
// //     addNode (oper, node_else);
// //
// //     if (strncmp (*pose, "else ", 5 /* len ("else ") */) == 0)
// //     {
// //         CompilerBlock_t* block_else = newBlock ();
// //         addBlock (parent, block_else);
// //         block_else->type = BLOCK_TYPE_ELSE;
// //
// //         if (parserUnion (inf, node_else, block_else) != PARSER_THIS_OK)
// //             SYNTAX_ERROR (inf, PARSER_ABSENCE_UNION, PARSER_THIS_ERROR);
// //     }
//     return PARSER_THIS_OK;
// }
//
// ParserOperStatus_t parserAssignOper (ParserPlaceInf_t* inf,
//                                      CompilerNode_t* parent,
//                                      CompilerBlock_t* parent_block,
//                                      NameTable_t* table_func)
// {
//     assert (inf);
//     assert (parent);
//     assert (parent_block);
//     assert (table_func);
//
//
//     if (parserVariable (inf, parent, parent_block) != PARSER_THIS_OK)
//         return PARSER_NOT_THIS;
//
//     if (**inf->pose != '=')
//         return PARSER_NOT_THIS;
//
//     ++ *inf->pose;
//     skipVoid (inf->pose);
//
//
//
//
//
// }
//
// // ---------------------------------------------------------------------------------------------------
// /**
//  @brief Функция считывания переменной
//  @param [in] inf
//  @param [in] parent
//  @param [in] block
// */
// ParserOperStatus_t parserVariable (ParserPlaceInf_t* inf,
//                                    CompilerNode_t* parent,
//                                    CompilerBlock_t* block)
// {
//     assert (inf);
//     assert (parent);
//     assert (block);
//
//     if (!isalpha (**(inf->pose)))
//         return PARSER_NOT_THIS;
//
//     char** pose = inf->pose;
//     size_t index = 0;
//
//     while (isalpha (*pose[index]) || isdigit (*pose[index]) || *pose[index] == '_')
//         index++;
//
//     char buffer_char = *pose[index];
//     *pose[index] = '\0';
//     char* name = strdup (*pose);
//     *pose[index] = buffer_char;
//
//     CompilerBlock_t* buf_block = block;
//     CompilerNode_t* variable = newNode ();
//     addNode (parent, variable);
//     variable->type = NODE_TYPE_VAR;
//     bool is_really = false;
//
//     while (buf_block->parent != NULL)
//     {
//         if (buf_block->table_var != NULL)
//         {
//             size_t index_table = 0;
//             index_table = nameTableFind (buf_block->table_var, name);
//             if (index_table != (size_t) -1)
//             {
//                 variable->value.index = index_table;
//                 CompilerNode_t* number_table = newNode ();
//                 number_table->type = NODE_TYPE_NUMBER_TABLE;
//                 number_table->value.index = buf_block->number;
//                 addNode (variable, number_table);
//                 is_really = true;
//                 break;
//             }
//         }
//         buf_block = buf_block->parent;
//     }
//     free (name);
//
//     if (is_really == false)
//         SYNTAX_ERROR (inf, PARSER_UNKNOWN_VARIABLE, PARSER_THIS_ERROR);
//
//     *pose += index;
//     skipVoid (pose);
//
//     return PARSER_THIS_OK;
// }
// // ---------------------------------------------------------------------------------------------------
//
// // ---------------------------------------------------------------------------------------------------
//
// ParserOperStatus_t parserFunction (ParserPlaceInf_t* inf,
//                                    CompilerNode_t* parent,
//                                    CompilerBlock_t* block,
//                                    NameTable_t* table_func)
// {
//     assert (inf);
//     assert (parent);
//     assert (block);
//     assert (table_func);
//
//     if (!isalpha (**(inf->pose)))
//         return PARSER_NOT_THIS;
//
//     /* Get name */
//     char** pose = inf->pose;
//     size_t index = 0;
//
//     while (isalpha (*pose[index]) || isdigit (*pose[index]) || *pose[index] == '_')
//         index++;
//
//     char buffer_char = *pose[index];
//     *pose[index] = '\0';
//     char* name = strdup (*pose);
//     *pose[index] = buffer_char;
//
//     size_t index_table = nameTableFind (table_func, name);
//     free (name);
//
//     if (index_table == (size_t) -1)
//         return PARSER_NOT_THIS;
//
//     /* Get arguments */
//     size_t amount_agrs = 0;
//
//     CompilerNode_t* node_func = newNode ();
//     addNode (parent, node_func);
//     node_func->type = NODE_TYPE_FUNC;
//     node_func->value.index = index_table;
//
//     *pose += index;
//     skipVoid (pose);
//     if (**pose != '(')
//         SYNTAX_ERROR (inf, PARSER_ABSENCE_BRACKET, PARSER_THIS_ERROR);
//
//     while (true)
//     {
//         skipVoid (pose);
//         if (isalpha (**pose) || isdigit (**pose))
//         {
//             if (parserExpresion (inf, node_func, block, table_func) != PARSER_THIS_OK)
//                 SYNTAX_ERROR (inf, PARSER_ABSENCE_EXPRESION, PARSER_THIS_ERROR);
//
//             amount_args++;
//             skipVoid (pose);
//             if (**pose == ',')
//                 ++ *pose;
//         }
//         else if (**pose == ')')
//         {
//             ++ *pose;
//             break;
//         }
//         else
//             SYNTAX_ERROR (inf, PARSER_ABSENCE_BRACKET, PARSER_THIS_ERROR);
//     }
//     // TODO: check amount agrs
//     // if (amount_agrs !=
//
//     skipVoid (pose);
//
//     return 0;
// }
// // ---------------------------------------------------------------------------------------------------
//
// // ---------------------------------------------------------------------------------------------------
//
// ParserOperStatus_t parserExtVar (ParserPlaceInf_t* inf,
//                                  CompilerNode_t* parent,
//                                  CompilerBlock_t* block)
// {
//     assert (inf);
//     assert (parent);
//     assert (block);
//
//
//
// }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// int addBlock (CompilerBlock_t* parent,
//               CompilerBlock_t* child)
// {
//     assert (parent);
//     assert (child);
//
//     CompilerBlock_t** buffer = NULL;
//     if (parent->children == NULL)
//     {
//         buffer = (CompilerBlock_t**) calloc (1, sizeof (CompilerBlock_t*));
//         if (buffer == NULL)
//             EXIT_FUNC("NULL calloc", 1);
//         child->amount_children = 0;
//     }
//     else
//     {
//         buffer = (CompilerBlock_t**) realloc (parent->children,
//                 sizeof (CompilerBlock_t*) * (parent->amount_children + 1));
//         if (buffer == NULL)
//             EXIT_FUNC("NULL realloc", 1);
//     }
//
//     buffer[parent->amount_children] = child;
//     child->parent = parent;
//     parent->amount_children++;
//     parent->children = buffer;
//
//     return 0;
// }
//
// int addNode (CompilerNode_t* parent,
//              CompilerNode_t* child)
// {
//     assert (parent);
//     assert (child);
//
//     CompilerNode_t** buffer = NULL;
//     if (parent->children == NULL)
//     {
//         buffer = (CompilerNode_t**) calloc (1, sizeof (CompilerNode_t*));
//         if (buffer == NULL)
//             EXIT_FUNC("NULL calloc", 1);
//         parent->amount_children = 0;
//     }
//     else
//     {
//         buffer = (CompilerNode_t**) realloc (parent->children,
//                   (parent->amount_children + 1) * sizeof (CompilerNode_t*));
//         if (buffer == NULL)
//             EXIT_FUNC("NULL realloc", 1);
//     }
//     buffer[parent->amount_children] = child;
//     child->parent = parent;
//     parent->amount_children++;
//     parent->children = buffer;
//
//     return 0;
// }
//
// int skipVoid (char** str)
// {
//     assert (str);
//     assert (*str);
//
//     while (**str == ' ' || **str == '\t' ||
//            **str == '\n' || **str == '\r')
//         ++ *str;
//     return 0;
// }
//
// // ---------------------------------------------------------------------------------------------------
// CompilerBlock_t* newBlock ()
// {
//     CompilerBlock_t* block = (CompilerBlock_t*) calloc (1, sizeof (CompilerBlock_t));
//     if (block == NULL)
//         EXIT_FUNC("NULL calloc", NULL);
//
//     block->amount_children = 0;
//     block->children = NULL;
//     block->free_memory = 0;
//     block->parent = NULL;
//     block->table_var = NULL;
//     block->type = BLOCK_TYPE_GLOBAL;
//     block->number = AMOUNT_BLOCKS;
//     AMOUNT_BLOCKS++;
//
//     return block;
// }
// // ---------------------------------------------------------------------------------------------------
//
// // ---------------------------------------------------------------------------------------------------
//
// CompilerNode_t* newNode ()
// {
//     CompilerNode_t* node = (CompilerNode_t*) calloc (1, sizeof (CompilerNode_t));
//     if (node == NULL)
//         EXIT_FUNC("NULL calloc", NULL);
//     return node;
// }
// // ---------------------------------------------------------------------------------------------------
//


