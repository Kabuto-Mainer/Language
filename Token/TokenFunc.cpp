#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "TokenFunc.h"
#include "VectorFunc.h"
#include "SupportFunc.h"
#include "TypesOfType.h"
#include "TypesConst.h"


// ---------------------------------------------------------------------------------------------------
int skipVoid (TokenContextInf_t* inf)
{
    assert (inf);
    assert (inf->pose);

    char** str = inf->pose;
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
    TokenContextInf_t inf = {
        .pose = str,
        .line = 0};

    while (true)
    {
        skipVoid (&inf);
        if (**(inf.pose) == '\0' || **(inf.pose) == '$')
            break;

        if (tokenPunct (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenMathOper (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenNum (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenKeyWord (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenIndent (&inf, vector) == PARSER_THIS_OK)
            continue;

        EXIT_FUNC ("UNKNOWN SYNTAX", 1);
    }
    // token
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Status_t tokenPunct (TokenContextInf_t* inf,
                     TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    printf ("Punct\n");
    printf ("%s", *inf->pose);
    Node_t node = {
        .type = NODE_TYPE_PUNCT,
        .parent = NULL,
        .amount_children = 0,
        .children = NULL,
    };

    switch (**inf->pose)
    {
        case ('('):    { node.value.punct = PUNCT_LEFT_ROUND; break; }
        case (')'):    { node.value.punct = PUNCT_RIGHT_ROUND; break; }
        case ('@'):    { node.value.punct = PUNCT_DOG; break; }
        case (','):    { node.value.punct = PUNCT_COMMA; break; }
        case ('\n'):   { node.value.punct = PUNCT_END_STR; inf->line++; break; }
        case ('#'):    { node.value.punct = PUNCT_H; break; }
        case ('"'):    { node.value.punct = PUNCT_QUOT; break; }
        default: return PARSER_NOT_THIS;
    }
    ++ *inf->pose;
    vectorAdd (vector, node);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Status_t tokenNum (TokenContextInf_t* inf,
                   TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    printf ("Num\n");
    printf ("%s", *inf->pose);
    if (!isdigit (**inf->pose) || (**inf->pose == '-' && !isdigit (*(*inf->pose + 1))))
        return PARSER_NOT_THIS;

    int value = 0;
    int len = 0;
    sscanf (*(inf->pose), "%d%n", &value, &len);

    Node_t node = {
        .type = NODE_TYPE_NUM,
        .parent = NULL,
        .amount_children = 0,
        .children = NULL,
        .value = {.num = value}
    };
    vectorAdd (vector, node);

    *(inf->pose) += len;
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Status_t tokenIndent (TokenContextInf_t* inf,
                      TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    printf ("Indent\n");
    printf ("%s", *inf->pose);
    if (!isalpha (**inf->pose))
        return PARSER_NOT_THIS;

    size_t len = 0;
    char** str = inf->pose;
    while (isalpha ((*str)[len]) || isdigit ((*str)[len]) || (*str)[len] == '_')
        len++;

    char buffer_char = (*str)[len];
    (*str)[len] = '\0';
    char* name = strdup (*str);
    (*str)[len] = buffer_char;

    Node_t node = {
        .type = NODE_TYPE_INDENT,
        .parent = NULL,
        .amount_children = 0,
        .children = NULL,
        .value = {.name = name }
    };
    vectorAdd (vector, node);

    *str += len;
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Status_t tokenKeyWord (TokenContextInf_t* inf,
                       TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    printf ("KeyWord\n");
    printf ("%s", *inf->pose);
    size_t len = 0;
    char** str = inf->pose;
    while (isalpha((*str)[len]))
    {
        // printf ("%c", (*str)[len]);
        len++;
    }

    char buffer_char = (*str)[len];
    (*str)[len] = '\0';

    printf ("STR: %s\n", *str);
    Node_t node = {
        .type = NODE_TYPE_KEY_WORD,
        .parent = NULL,
        .amount_children = 0,
        .children = NULL,
    };

    if      (strcmp ("alloc", *str) == 0)    node.value.key = KEY_EXTERN_VAR;
    else if (strcmp ("server", *str) == 0)   node.value.key = KEY_EXTERN_FUNC;
    else if (strcmp ("trig", *str) == 0)     node.value.key = KEY_IF;
    else if (strcmp ("nexttrig", *str) == 0) node.value.key = KEY_ELIF;
    else if (strcmp ("default", *str) == 0)  node.value.key = KEY_ELSE;
    else if (strcmp ("loop", *str) == 0)     node.value.key = KEY_WHILE;
    else if (strcmp ("ret", *str) == 0)      node.value.key = KEY_RETURN;
    else
    {
        (*str)[len] = buffer_char;
        return PARSER_NOT_THIS;
    }
    (*str)[len] = buffer_char;
    *str += len;
    vectorAdd (vector, node);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Status_t tokenMathOper (TokenContextInf_t* inf,
                        TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    printf ("Math\n");
    printf ("%s", *inf->pose);
    size_t len = 0;
    char** str = inf->pose;
    while (ispunct((*str)[len]))
        len++;

    char buffer_char = (*str)[len];
    (*str)[len] = '\0';

    Node_t node = {
        .type = NODE_TYPE_OPER,
        .parent = NULL,
        .amount_children = 0,
        .children = NULL
    };

    if      (strcmp ("**", *str) == 0)  node.value.oper = OPER_POW;
    else if (strcmp ("<=", *str) == 0)  node.value.oper = OPER_COMP_LIT_EQUAL;
    else if (strcmp (">=", *str) == 0)  node.value.oper = OPER_COMP_BIG_EQUAL;
    else if (strcmp ("==", *str) == 0)  node.value.oper = OPER_COMP_EQUAL;
    else if (strcmp ("!=", *str) == 0)  node.value.oper = OPER_COMP_NOT_EQUAL;
    else if (len == 1)
    {
        switch (**str)
        {
            case ('+'):    { node.value.oper = OPER_ADD; break; }
            case ('-'):    { node.value.oper = OPER_SUB; break; }
            case ('*'):    { node.value.oper = OPER_MUL; break; }
            case ('/'):    { node.value.oper = OPER_DIV; break; }
            case ('<'):    { node.value.oper = OPER_COMP_ONLY_LIT; break; }
            case ('>'):    { node.value.oper = OPER_COMP_ONLY_BIG; break; }
            case ('='):
            {
                node.type = NODE_TYPE_KEY_WORD;
                node.value.key = KEY_ASSIGN;
                break;
            }
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
    *str += len;
    vectorAdd (vector, node);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int tokenDump (TokenVector_t* vector)
{
    assert (vector);

    for (size_t i = 0; i < vectorGetSize (vector); i++)
    {
        Node_t* node = vectorGetToken (vector, i);
        printf ("[%zu][%d]<", i, node->type);
        if (node->type == NODE_TYPE_INDENT)
            printf ("%s", node->value.name);
        else if (node->type == NODE_TYPE_OPER)
            printf ("%s", CHAR_OPER_TYPE[node->value.oper]);
        else if (node->type == NODE_TYPE_KEY_WORD)
            printf ("%s", CHAR_KEY_WORD[node->value.key]);
        else if (node->type == NODE_TYPE_PUNCT)
            printf ("%s", CHAR_PUNCT[node->value.punct]);
        else if (node->type == NODE_TYPE_NUM)
            printf ("%d", node->value.num);
        else
            printf ("block");
        printf (">\n");
    }
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int tokenOneDump (Node_t* node)
{
    assert (node);


    printf ("[%d]<", node->type);
    if (node->type == NODE_TYPE_INDENT)
        printf ("%s", node->value.name);
    else if (node->type == NODE_TYPE_OPER)
        printf ("%s", CHAR_OPER_TYPE[node->value.oper]);
    else if (node->type == NODE_TYPE_KEY_WORD)
        printf ("%s", CHAR_KEY_WORD[node->value.key]);
    else if (node->type == NODE_TYPE_PUNCT)
        printf ("%s", CHAR_PUNCT[node->value.punct]);
    else if (node->type == NODE_TYPE_NUM)
        printf ("%d", node->value.num);
    else
        printf ("block");
    printf (">\n");

    return 0;
}
// ---------------------------------------------------------------------------------------------------

