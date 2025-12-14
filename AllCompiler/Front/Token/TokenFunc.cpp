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
#include "ConfigLangConst.h"


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

        if (tokenNum (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenMathOper (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenPunct (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenKeyWord (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenIndent (&inf, vector) == PARSER_THIS_OK)
            continue;

        EXIT_FUNC ("UNKNOWN SYNTAX", 1);
    }
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Status_t tokenPunct (TokenContextInf_t* inf,
                     TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    Node_t node = {
        .type = NODE_TYPE_PUNCT,
        .parent = NULL,
        .amount_children = 0,
        .children = NULL,
    };

    switch (**inf->pose)
    {
        case ',':  { node.value.punct = PUNCT_COMMA; break; }
        case '(':  { node.value.punct = PUNCT_LEFT_ROUND; break; }
        case ')':  { node.value.punct = PUNCT_RIGHT_ROUND; break; }
        case '\\': { node.value.punct = PUNCT_NEXT_STR; break; }
        case '\n': { node.value.punct = PUNCT_END_STR; break; }
        case '@':  { node.value.punct = PUNCT_DOG; break; }
        case '#':  { node.value.punct = PUNCT_H; break; }
        case '\"': { node.value.punct = PUNCT_QUOT; break; }
        case '<':  { node.value.punct = PUNCT_LEFT_TANG; break; }
        case '>':  { node.value.punct = PUNCT_RIGHT_TANG; break; }
        case '~':  { node.value.punct = PUNCT_END_STR; break; }
        default: { return PARSER_NOT_THIS; }
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

    if ((**inf->pose == '-' && !isdigit (*(*inf->pose + 1))) ||
        (**inf->pose != '-' && !isdigit (**inf->pose)))
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

    if (!isalpha (**inf->pose))
        return PARSER_NOT_THIS;

    size_t len = 0;
    char** str = inf->pose;
    char buffer[MAX_TOKEN_LEN] = "";

    while (isalpha ((*str)[len]) ||
           isdigit ((*str)[len]) ||
           (*str)[len] == '_')
    {
        buffer[len] = (*str)[len];
        len++;
    }

    char* name = strdup (buffer);
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

    size_t len = 0;
    char** str = inf->pose;
    char buffer[MAX_TOKEN_LEN] = "";

    while (isalpha((*str)[len]) ||
           (*str)[len] == '_' ||
           (*str)[len] == '=')
    {
        buffer[len] = (*str)[len];
        len++;
    }

    Node_t node = {
        .type = NODE_TYPE_KEY_WORD,
        .parent = NULL,
        .amount_children = 0,
        .children = NULL,
    };

    bool is_real = false;
    for (size_t i = 0; i < sizeof (ALL_SYSTEM_WORD) / sizeof (ALL_SYSTEM_WORD[0]); i++)
    {
        if (strcmp (ALL_SYSTEM_WORD[i].name, buffer) == 0)
        {
            node.value.key = ALL_SYSTEM_WORD[i].value;
            is_real = true;
            break;
        }
    }
    if (!is_real)
        return PARSER_NOT_THIS;

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

    size_t len = 0;
    char* str = *(inf->pose);
    char buffer[MAX_TOKEN_LEN] = "";

    while (true)
    {
        bool is_correct = false;
        for (size_t i = 0; i < sizeof (OPER_SYMBOLS) / sizeof (OPER_SYMBOLS[0]); i++)
        {
            if (*str == OPER_SYMBOLS[i])
            {
                buffer[len++] = *str;
                str++;
                is_correct = true;
                break;
            }
        }
        if (is_correct == false)
            break;
    }

    Node_t node = {
        .type = NODE_TYPE_OPER,
        .parent = NULL,
        .amount_children = 0,
        .children = NULL
    };

    bool is_real = false;
    for (size_t i = 0; i < sizeof (ALL_OPER_WORD) / sizeof (ALL_OPER_WORD[0]); i++)
    {
        if (strcmp (ALL_OPER_WORD[i].name, buffer) == 0)
        {
            node.value.oper = ALL_OPER_WORD[i].value;
            is_real = true;
        }
    }

    if (is_real == false)
        return PARSER_NOT_THIS;

    *inf->pose = str;
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
        printf ("[%zu]", i);
        tokenOneDump (node);
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
    {
        for (size_t i = 0; i < sizeof (ALL_OPER_WORD) / sizeof (ALL_OPER_WORD[0]); i++)
        {
            if (node->value.oper == ALL_OPER_WORD[i].value)
                printf ("%s", ALL_OPER_WORD[i].name);
        }
    }
    else if (node->type == NODE_TYPE_KEY_WORD)
    {
        for (size_t i = 0; i < sizeof (ALL_SYSTEM_WORD) / sizeof (ALL_SYSTEM_WORD[0]); i++)
        {
            if (node->value.key == ALL_SYSTEM_WORD[i].value)
                printf ("%s", ALL_SYSTEM_WORD[i].name);
        }
    }
    else if (node->type == NODE_TYPE_PUNCT)
    {
        for (size_t i = 0; i < sizeof (ALL_PUNCT_WORD) / sizeof (ALL_PUNCT_WORD[0]); i++)
        {
            if (node->value.punct == ALL_PUNCT_WORD[i].value)
                printf ("%s", ALL_PUNCT_WORD[i].name);
        }
    }
    else if (node->type == NODE_TYPE_NUM)
        printf ("%d", node->value.num);
    else
        printf ("block");

    printf (">\n");

    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int tokenOneDump (Node_t* node, const char* reason)
{
    assert (node);
    assert (reason);

    printf ("[%d]<", node->type);
    if (node->type == NODE_TYPE_INDENT)
        printf ("%s", node->value.name);

    else if (node->type == NODE_TYPE_OPER)
    {
        for (size_t i = 0; i < sizeof (ALL_OPER_WORD) / sizeof (ALL_OPER_WORD[0]); i++)
        {
            if (node->value.oper == ALL_OPER_WORD[i].value)
                printf ("%s", ALL_OPER_WORD[i].name);
        }
    }
    else if (node->type == NODE_TYPE_KEY_WORD)
    {
        for (size_t i = 0; i < sizeof (ALL_SYSTEM_WORD) / sizeof (ALL_SYSTEM_WORD[0]); i++)
        {
            if (node->value.key == ALL_SYSTEM_WORD[i].value)
                printf ("%s", ALL_SYSTEM_WORD[i].name);
        }
    }
    else if (node->type == NODE_TYPE_PUNCT)
    {
        for (size_t i = 0; i < sizeof (ALL_PUNCT_WORD) / sizeof (ALL_PUNCT_WORD[0]); i++)
        {
            if (node->value.punct == ALL_PUNCT_WORD[i].value)
                printf ("%s", ALL_PUNCT_WORD[i].name);
        }
    }
    else if (node->type == NODE_TYPE_NUM)
        printf ("%d", node->value.num);
    else
        printf ("block");

    printf (">%s\n", reason);

    return 0;
}
// ---------------------------------------------------------------------------------------------------

