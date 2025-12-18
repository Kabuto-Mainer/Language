#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "TokenFunc.h"
#include "VectorFunc.h"
#include "SupportFunc.h"
#include "AllLexeme.h"
#include "NodeType.h"
#include "TypesOfType.h"
#include "StringTable.h"
#include "AllLexeme.h"

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
                 StringTable_t* table_str,
                 TokenVector_t* vector)
{
    assert (buffer);
    assert (table_str);
    assert (vector);

    char** str = &buffer;
    TokenContextInf_t inf =
    {
        .pose = str,
        .line = 0,
        .table_str = table_str,
        .lexeme_data = LexDB_Create ()
    };

    while (true)
    {
        skipVoid (&inf);
        if (**(inf.pose) == '\0' || **(inf.pose) == '$')
            break;

        if (tokenComment (&inf) == PARSER_THIS_OK)
            continue;

        if (tokenNumber (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenOper (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenPunct (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenKeyWord (&inf, vector) == PARSER_THIS_OK)
            continue;

        if (tokenIdent (&inf, vector) == PARSER_THIS_OK)
            continue;

        EXIT_FUNC ("UNKNOWN SYNTAX", 1);
    }

    LexDB_Destroy (inf.lexeme_data);
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Status_t tokenComment (TokenContextInf_t* inf)
{
    assert (inf);
    if (**inf->pose != '#')
        return PARSER_NOT_THIS;

    while (**inf->pose != '\n' && **inf->pose != '\0')
        ++ *inf->pose;

    if (**inf->pose == '\n')
        ++ *inf->pose;

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t tokenNumber (TokenContextInf_t* inf,
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

    Node_t node =
    {
        .type_node = NODE_NUM,
        .parent = NULL,
        .children = NULL,
        .amount_children = 0,
    };
    node.val.num = value;
    vectorAdd (vector, node);

    *(inf->pose) += len;
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t tokenIdent (TokenContextInf_t* inf,
                     TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    // printf ("Ident\n");
    if (!isalpha (**inf->pose) && **inf->pose != '_')
        return PARSER_SYNTAX_ERROR;

    int len = 0;
    char buffer[TOKEN_MAX_LEN] = "";

    while ((isalpha ((*inf->pose)[len]) ||
            isdigit ((*inf->pose)[len]) ||
            (*inf->pose)[len] == '_') &&
           len < TOKEN_MAX_LEN - 1)
    {
        buffer[len] = (*inf->pose)[len];
        len++;
    }

    buffer[len] = '\0';

    Node_t token =
    {
        .type_node = NODE_ID_RAW,
        .parent = NULL,
        .children = NULL,
        .amount_children = 0,
    };
    token.val.id_raw.name = StrT_AddString (inf->table_str, buffer);

    vectorAdd (vector, token);
    *inf->pose += len;

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t tokenPunct (TokenContextInf_t* inf,
                     TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    int len = 0;
    Punct_t punct_type = PUNCT_ENDSTRING;

    if (LexDB_FindPunct (inf->lexeme_data, *inf->pose,
                         &len, &punct_type) == 0)
    {
        Node_t token =
        {
            .type_node = NODE_PUNCT,
        };
        token.val.punct = punct_type;

        vectorAdd (vector, token);
        *inf->pose += len;
        return PARSER_THIS_OK;
    }

    return PARSER_NOT_THIS;
}

// ---------------------------------------------------------------------------------------------------
Status_t tokenOper (TokenContextInf_t* inf,
                    TokenVector_t* vector)
{
    assert (inf);
    assert (vector);

    int len = 0;
    Oper_t op_type = OPER_ADD;

    if (LexDB_FindOper (inf->lexeme_data, *inf->pose,
                        &len, &op_type) == 0)
    {
        // printf ("OPER\n");
        // printf ("%s\n", *inf->pose);
        Node_t token =
        {
            .type_node = NODE_OPER,
        };
        token.val.oper.code = op_type;

        vectorAdd (vector, token);
        *inf->pose += len;
        return PARSER_THIS_OK;
    }

    return PARSER_NOT_THIS;
}

// ---------------------------------------------------------------------------------------------------
Status_t tokenKeyWord (TokenContextInf_t* inf,
                       TokenVector_t* vector)
{
    assert (inf);
    assert (vector);


    if (!isalpha (**inf->pose))
        return PARSER_SYNTAX_ERROR;

    int len = 0;
    char buffer[TOKEN_MAX_LEN] = "";

    while (isalpha ((*inf->pose)[len]) &&
           len < TOKEN_MAX_LEN - 1)
    {
        buffer[len] = (*inf->pose)[len];
        len++;
    }

    buffer[len] = '\0';
    int key_id = LexDB_FindKeyword (inf->lexeme_data, buffer);
    if (key_id == -1)
        return PARSER_NOT_THIS;

    Node_t token =
    {
        .type_node = NODE_KEY,
    };
    token.val.key = key_id;

    vectorAdd (vector, token);
    *inf->pose += len;

    return PARSER_THIS_OK;
}


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

    printf ("[%d]<", node->type_node);
    if (node->type_node == NODE_ID_RAW)
        printf ("%s", node->val.id_raw.name->string);

    else if (node->type_node == NODE_PUNCT)
        printf ("%s", PUNCTUATION[node->val.punct].lexeme);

    else if (node->type_node == NODE_OPER)
        printf ("%s", OPERATIONS[node->val.oper.code].lexeme);

    else if (node->type_node == NODE_KEY)
        printf ("%s", KEYWORDS[node->val.key].lexeme);

    else if (node->type_node == NODE_NUM)
        printf ("%d", node->val.num);

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

    printf ("[%d]<", node->type_node);
    if (node->type_node == NODE_ID_RAW)
        printf ("%s", node->val.id_raw.name->string);

    else if (node->type_node == NODE_PUNCT)
        printf ("%s", PUNCTUATION[node->val.punct].lexeme);

    else if (node->type_node == NODE_OPER)
        printf ("%s", OPERATIONS[node->val.oper.code].lexeme);

    else if (node->type_node == NODE_KEY)
        printf ("%s", KEYWORDS[node->val.key].lexeme);

    else if (node->type_node == NODE_NUM)
        printf ("%d", node->val.num);

    else
        printf ("block");

    printf (">%s\n", reason);

    return 0;
}
// ---------------------------------------------------------------------------------------------------

