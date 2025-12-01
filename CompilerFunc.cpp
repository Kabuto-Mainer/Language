#include <string.h>
#include <assert.h>

#include "Support/SupportFunc.h"
#include "NameTable/NameTableFunc.h"
// #include "Vector/VectorFunc.h"
#include "Token/TokenFunc.h"

struct CompilerPlaceInf_t
{
    Token_t* token;
    size_t cur_index;
    size_t capacity;
};


int compiler (CompilerPlaceInf_t* inf)
{
    assert (inf);
    // if (*(inf->token

}


CompilerNode_t* compilerExpresion (CompilerPlaceInf_t* inf,
                       CompilerBlock_t* block,
                       NameTable_t* table_func);
{
    assert (inf);
    assert (block);
    assert (table_func);

    CompilerNode_t* node_expr = newNode ();
    compilerAddSub (inf, block, table_func, node_expr);


}

CompilerNode_t* compilerAddSub (CompilerPlaceInf_t* inf,
                    CompilerBlock_t* block,
                    const NameTable_t* table_func);
{
    assert (inf);
    assert (block);
    assert (table_func);

    CompilerNode_t* node = compilerMulDiv (inf, block, table_func);
}

ParserStatus_t compilerMulDiv (CompilerPlaceInf_t* inf,
                               CompilerBlock_t* block,
                               const NameTable_t* table_func,
                               CompilerNode_t* parent)
{
    assert (inf);
    assert (block);
    assert (table_func);
    assert (parent);

    CompilerNode_t* node = newNode ();
    addNode (parent, node);
    if (compilerPower (inf, block, table_func, node) != PARSER_THIS_OK)
        SYNTAX_ERROR ();

    if (inf->token->type != TOKEN_MUL &&
        inf->token->type != TOKEN_DIV)
    {
        replaceChildren (parent, node);
        return PARSER_THIS_OK;
    }

    node->type = NODE_TYPE_OPER;

}

ParserStatus_t compilerPower (CompilerPlaceInf_t* inf,
                              CompilerBlock_t* block,
                              const NameTable_t* table_func,
                              CompilerNode_t* parent)
{
    assert (inf);
    assert (block);
    assert (table_func);
    assert (parent);

    CompilerNode_t* node = newNode ();
    addNode (parent, node);
    if (compilerValue (inf, block, table_func, node) != PARSER_THIS_OK)
        SYNTAX_ERROR ();

    if (inf->token->type != TOKEN_POW)
    {
        replaceChildren (parent, node);
        return PARSER_THIS_OK;
    }

    node->type = NODE_TYPE_OPER;
    node->value.oper = OPER_POW;

    getNextToken (inf);
    if (compilerNumber (inf, block, table_func, node) != PARSER_THIS_OK)
        SYNTAX_ERROR ();

    return PARSER_THIS_OK;
}

ParserStatus_t compilerValue (CompilerPlaceInf_t* inf,
                              CompilerBlock_t* block,
                              const NameTable_t* table_func,
                              CompilerNode_t* parent)
{
    assert (inf);
    assert (block);
    assert (table_func);
    assert (parent);

    if (inf->token->type == TOKEN_LEFT_ROUND)
    {
        compilerExpresion (inf, block, table_func, parent);
        if (inf->token->type != TOKEN_RIGHT_ROUND)
            SYNTAX_ERROR (inf );

        getNextToken (inf);
        return PARSER_THIS_OK;
    }
    if (compilerNumber (inf, block, table_func, parent) == PARSER_THIS_OK)
        return PARSER_THIS_OK;

    if (compilerFunction (inf, block, table_func, parent) == PARSER_THIS_OK)
        return PARSER_THIS_OK;

    if (compilerVariable (inf, block, table_func, parent) == PARSER_THIS_OK)
        return PARSER_THIS_OK;

    SYNTAX_ERROR ();
}

ParserStatus_t compilerFunction (CompilerPlaceInf_t* inf,
                                 CompilerBlock_t* block,
                                 NameTable_t* table_func,
                                 CompilerNode_t* parent)
{
    assert (inf);
    assert (block);
    assert (table_func);

    if (inf->token->type != TOKEN_INDENT)
        return PARSER_NOT_THIS;

    char* name = (char*) calloc (inf->token->len_name + 1, sizeof (char));
    if (name == NULL)
        EXIT_FUNC ("NULL calloc", PARSER_THIS_ERROR);

    strncpy (name, inf->token->start, inf->token->len_name);

    size_t index = nameTableFind (table_func, name);
    free (name);

    if (index == (size_t) -1)
        return PARSER_NOT_THIS;

    getNextToken (inf);
    if (inf->token->type != TOKEN_LEFT_ROUND)
        SYNTAX_ERROR ();

    int amount_args = 0;
    CompilerNode_t* node_func = newNode ();
    node_func->type = NODE_TYPE_CALL_FUNC;
    addNode (parent, node_func);

    while (true)
    {
        if (compilerExpresion (inf, block, table_func, node_func) != PARSER_THIS_OK)
            SYNTAX_ERROR ();

        amount_args++;
        if (inf->token->type != TOKEN_COMMA)
            break;
    }
    if (amount_args != table_func->data[index].value.func->amount_args)
        SYNTAX_ERROR ();

    if (inf->token->type != TOKEN_RIGHT_ROUND)
        SYNTAX_ERROR ();

    getNextToken (inf);
    return PARSER_THIS_OK;
}

ParserStatus_t compilerVariable (CompilerPlaceInf_t* inf,
                                 CompilerBlock_t* block,
                                 CompilerNode_t* parent)
{
    assert (inf);
    assert (block);
    assert (parent);

    char* name = (char*) calloc (inf->token->len_name + 1, sizeof (char));
    if (name == NULL)
        EXIT_FUNC ("NULL calloc", PARSER_THIS_ERROR);

    strncpy (name, inf->token->start, inf->token->len_name);

    CompilerBlock_t* buffer_block = block;
    size_t index = 0;
    int number_table = 0;
    bool is_real = false;

    while (buffer_block != NULL)
    {
        if (buffer_block->table_var != NULL)
        {
            index = nameTableFind (buffer_block->table_var, name);
            if (index != (size_t) -1)
            {
                number_table = buffer_block->number;
                is_real = true;
                break;
            }
        }
    }
    free (name);
    if (is_real == false)
        SYNTAX_ERROR ();

    CompilerNode_t* variable = newNode ();
    addNode (parent, variable);
    variable->type = NODE_TYPE_VAR;
    variable->value.index = number_table;

    CompilerNode_t* node_index_in_table = newNode ();
    addNode (variable, node_index_in_table);
    node_index_in_table->type = NODE_TYPE_INDEX_IN_TABLE;
    node_index_in_table->value.index = index;

    getNextToken (inf);
    return PARSER_THIS_OK;
}

ParserStatus_t compilerNumber (CompilerPlaceInf_t* inf,
                               CompilerBlock_t* block,
                               CompilerNode_t* parent)
{
    assert (inf);
    assert (block);
    assert (parent);

    if (inf->token->type != TOKEN_NUMBER)
        return PARSER_NOT_THIS;

    CompilerNode_t* node_num = newNode ();
    addNode (parent, node_num);

    node_num->type = NODE_TYPE_CONST;
    node_num->value.num = inf->token->value;

    getNexttoken (inf);
    return PARSER_THIS_OK;
}

int getNextToken (CompilerPlaceInf_t* inf)
{
    assert (inf);
    if (inf->cur_index < inf->capacity)
    {
        inf->token = inf->token + 1;
        inf->cur_index++;
        return 0;
    }
    return 1;
}

