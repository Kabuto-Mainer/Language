#include <string.h>
#include <assert.h>

#include "Support/SupportFunc.h"
#include "NameTable/NameTableFunc.h"
#include "Token/TokenType.h"

// ---------------------------------------------------------------------------------------------------
static int AMOUNT_BLOCKS = 0;
// ---------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------
int compiler (CompilerPlaceInf_t* inf,
              NameTable_t* table_func)
{
    assert (inf);
    assert (table_func);

    CompilerBlock_t* global_block = newBlock ();
    global_block->type = BLOCK_TYPE_GLOBAL;

    CompilerNode_t* global_node = newNode ();
    global_node->type = NODE_TYPE_GLOBAL;

    while (inf->cur_index != inf->capacity)
    {
        if (compilerExtVar (inf, global_block, table_func) == PARSER_THIS_OK)
            continue;

        printf ("Syntax Error");
        break;
    }

    deleteBlock (global_block);
    deleteNode (global_node);

    return 0;
}
// ---------------------------------------------------------------------------------------------------

/*
// ---------------------------------------------------------------------------------------------------
ParserStatus_t compilerOper (CompilerPlaceInf_t* inf,
                             CompilerBlock_t* block,
                             const NameTable_t* table_func)
{
    assert (inf);
    assert (block);
    assert (table_func);

    // if (inf->token->type == TOKEN_IF)
    // {



}
// ---------------------------------------------------------------------------------------------------
*/
/*
// ---------------------------------------------------------------------------------------------------
ParserStatus_t compiler
// ---------------------------------------------------------------------------------------------------
*/

/*
// ---------------------------------------------------------------------------------------------------
ParserStatus_t compilerIfOper (CompilerPlaceInf_t* inf,
                               CompilerBlock_t* block,
                               const NameTable_t* table_func,
                               CompilerNode_t* parent)
{
    assert (inf);
    assert (block);
    assert (table_func);

    if (inf->token->type != TOKEN_IF)
        return PARSER_NOT_THIS;

    getNextToken (inf);
    if (inf->token->type != TOKEN_DOG)
        SYNTAX_ERROR ();

    CompilerNode_t* node = newNode ();
    addNode (parent, node);
    if (compilerExpresion (inf, block, table_func, node) != PARSER_THIS_OK)
        SYNTAX_ERROR ();

    if (inf->token->type != TOKEN_DOG)
        SYNTAX_ERROR ();

    getNextToken (inf);
    if (inf->token->type != TOKEN_LEFT_ROUND)
        SYNTAX_ERROR ();

    CompilerBlock_t* new_block = newBlock ();
    addBlock (block, new_block);

    if (compilerUnion (inf, new_block, table_func, node) != PARSER_THIS_OK)
        SYNTAX_ERROR ();

    getNextToken (inf);
    if (inf->token->type != TOKEN_RIGHT_ROUND)
        SYNTAX_ERROR ();

    getNextToken (inf);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------
*/

// ---------------------------------------------------------------------------------------------------
ParserStatus_t compilerExtVar (CompilerPlaceInf_t* inf,
                               CompilerBlock_t* block,
                               const NameTable_t* table_func)
{
    assert (inf);
    assert (block);
    assert (table_func);

    if (inf->token->type != TOKEN_EXTERN_VAR)
        return PARSER_NOT_THIS;

    getNextToken (inf);
    if (inf->token->type != TOKEN_INDENT)
        SYNTAX_ERROR ();

    char* name = (char*) calloc (inf->token->len_name + 1, sizeof (char));
    if (name == NULL)
        EXIT_FUNC ("NULL calloc", PARSER_THIS_ERROR);

    strncpy (name, inf->token->start, inf->token->len_name);

    CompilerBlock_t* buffer_block = NULL;
    bool is_correct = true;

    while (buffer_block != NULL)
    {
        if (buffer_block->table_var != NULL)
        {
            if (nameTableFind (buffer_block->table_var, name) != (size_t) -1)
            {
                is_correct = false;
                break;
            }
        }
        buffer_block = buffer_block->parent;
    }
    if (nameTableFind (table_func, name) != (size_t) -1)
        is_correct = false;

    free (name);
    if (is_correct == false)
        SYNTAX_ERROR ();

    getNextToken (inf);
    if (inf->token->type != TOKEN_END_STR)
        SYNTAX_ERROR ();

    getNextToken (inf);
    CompilerVar_t* var_struct = (CompilerVar_t*) calloc (1, sizeof (CompilerVar_t));
    if (var_struct == NULL)
        EXIT_FUNC ("NULL calloc", PARSER_NOT_THIS);

    var_struct->block = block;
    var_struct->value = 0.0;

    nameTableAdd (table_func, name,
    (union NameTableValue_t) {.var = var_struct}, inf->token->len_name);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
ParserStatus_t compilerExpresion (CompilerPlaceInf_t* inf,
                                  CompilerBlock_t* block,
                                  NameTable_t* table_func,
                                  CompilerNode_t* parent)
{
    assert (inf);
    assert (block);
    assert (table_func);
    assert (parent);

    CompilerNode_t* node_expr = newNode ();
    if (compilerAddSub (inf, block, table_func, node_expr) != PARSER_THIS_OK)
        SYNTAX_ERROR ();

    CompilerOperType_t oper  = OPER_COMP_NOT_EQUAL;
    if (inf->token->type == TOKEN_BIG_EQUAL)
        oper = OPER_COMP_BIG_EQUAL;
    else if (inf->token->type == TOKEN_LIT_EQUAL)
        oper = OPER_COMP_LIT_EQUAL;
    else if (inf->token->type == TOKEN_ONLY_BIG)
        oper = OPER_COMP_ONLY_BIG;
    else if (inf->token->type == TOKEN_ONLY_LIT)
        oper = OPER_COMP_ONLY_LIT;
    else if (inf->token->type == TOKEN_EQUAL)
        oper = OPER_COMP_EQUAL;
    else if (inf->token->type == TOKEN_NOT_EQUAL)
        oper = OPER_COMP_NOT_EQUAL;
    else
    {
        replaceChildren (parent, node_expr);
        return PARSER_THIS_OK;
    }

    getNextToken (inf);
    if (compilerAddSub (inf, block, table_func, node_expr) != PARSER_THIS_OK)
        SYNTAX_ERROR ();

    node_expr->type = NODE_TYPE_OPER;
    node_expr->value.oper = oper;
    nodeAdd (parent, node_expr);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
ParserStatus_t compilerAddSub (CompilerPlaceInf_t* inf,
                               CompilerBlock_t* block,
                               const NameTable_t* table_func,
                               CompilerNode_t* parent)
{
    assert (inf);
    assert (block);
    assert (table_func);
    assert (parent);

    CompilerNode_t* node = newNode ();
    if (compilerMulDiv (inf, block, table_func, node) != PARSER_THIS_OK)
        SYNTAX_ERROR ();

    if (inf->token->type != TOKEN_ADD &&
        inf->token->type != TOKEN_SUB)
    {
        replaceChildren (parent, node);
        return PARSER_THIS_OK;
    }

    CompilerNode_t* node_two_oper = NULL;
    bool is_two_oper = false;
    TokenType_t first_oper_token = inf->token->type;
    node->type = NODE_TYPE_OPER;

    if (inf->token->type == TOKEN_ADD)
        node->value.oper = OPER_ADD;
    else
        node->value.oper = OPER_SUB;

    while (true)
    {
        if (inf->token->type != TOKEN_ADD || inf->token->type != TOKEN_SUB)
            break;

        if (first_oper_token != inf->token->type && !is_two_oper)
        {
            is_two_oper = true;
            node_two_oper = newNode ();
            node_two_oper->type = NODE_TYPE_OPER;
            addNode (node_two_oper, node);

            if (first_oper_token == TOKEN_ADD)
                node_two_oper->value.oper = OPER_SUB;
            else
                node_two_oper->value.oper = OPER_ADD;
        }
        if (first_oper_token != inf->token->type)
        {
            if (compilerPower (inf, block, table_func, node_two_oper) != PARSER_THIS_OK)
                SYNTAX_ERROR ();
        }
        else
        {
            if (compilerPower (inf, block, table_func, node) != PARSER_THIS_OK)
                SYNTAX_ERROR ();
        }
    }
    if (is_two_oper)
        addNode (parent, node_two_oper);
    else
        addNode (parent, node);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
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
    if (compilerPower (inf, block, table_func, node) != PARSER_THIS_OK)
        SYNTAX_ERROR ();

    if (inf->token->type != TOKEN_MUL &&
        inf->token->type != TOKEN_DIV)
    {
        replaceChildren (parent, node);
        return PARSER_THIS_OK;
    }

    CompilerNode_t* node_two_oper = NULL;
    bool is_two_oper = false;
    TokenType_t first_oper_token = inf->token->type;
    node->type = NODE_TYPE_OPER;

    if (inf->token->type == TOKEN_MUL)
        node->value.oper = OPER_MUL;
    else
        node->value.oper = OPER_DIV;

    while (true)
    {
        if (inf->token->type != TOKEN_MUL || inf->token->type != TOKEN_DIV)
            break;

        if (first_oper_token != inf->token->type && !is_two_oper)
        {
            is_two_oper = true;
            node_two_oper = newNode ();
            node_two_oper->type = NODE_TYPE_OPER;
            addNode (node_two_oper, node);

            if (first_oper_token == TOKEN_MUL)
                node_two_oper->value.oper = OPER_DIV;
            else
                node_two_oper->value.oper = OPER_MUL;
        }
        if (first_oper_token != inf->token->type)
        {
            if (compilerPower (inf, block, table_func, node_two_oper) != PARSER_THIS_OK)
                SYNTAX_ERROR ();
        }
        else
        {
            if (compilerPower (inf, block, table_func, node) != PARSER_THIS_OK)
                SYNTAX_ERROR ();
        }
    }
    if (is_two_oper)
        addNode (parent, node_two_oper);
    else
        addNode (parent, node);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
CompilerNode_t* newNode ()
{
    CompilerNode_t* node = (CompilerNode_t*) calloc (1, sizeof (CompilerNode_t));
    if (node == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    return node;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
CompilerBlock_t* newBlock ()
{
    CompilerBlock_t* block = (CompilerBlock_t*) calloc (1, sizeof (block));
    if (block == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    NameTable_t* table = (NameTable_t*) calloc (1, sizeof (NameTable_t));
    if (table == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    block->amount_children = 0;
    block->number = AMOUNT_BLOCKS++;

    return block;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int addBlock (CompilerBlock_t* parent,
              CompilerBlock_t* child)
{
    assert (parent);
    assert (child);

    CompilerBlock_t** buffer = NULL;
    if (parent->children == NULL)
    {
        buffer = (CompilerBlock_t**) calloc (1, sizeof (CompilerBlock_t*));
        if (buffer == NULL)
            EXIT_FUNC("NULL calloc", 1);
        child->amount_children = 0;
    }
    else
    {
        buffer = (CompilerBlock_t**) realloc (parent->children,
                sizeof (CompilerBlock_t*) * (parent->amount_children + 1));
        if (buffer == NULL)
            EXIT_FUNC("NULL realloc", 1);
    }

    buffer[parent->amount_children] = child;
    child->parent = parent;
    parent->amount_children++;
    parent->children = buffer;

    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int addNode (CompilerNode_t* parent,
             CompilerNode_t* child)
{
    assert (parent);
    assert (child);

    CompilerNode_t** buffer = NULL;
    if (parent->children == NULL)
    {
        buffer = (CompilerNode_t**) calloc (1, sizeof (CompilerNode_t*));
        if (buffer == NULL)
            EXIT_FUNC("NULL calloc", 1);
        parent->amount_children = 0;
    }
    else
    {
        buffer = (CompilerNode_t**) realloc (parent->children,
                  (parent->amount_children + 1) * sizeof (CompilerNode_t*));
        if (buffer == NULL)
            EXIT_FUNC("NULL realloc", 1);
    }
    buffer[parent->amount_children] = child;
    child->parent = parent;
    parent->amount_children++;
    parent->children = buffer;

    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int replaceChildren (CompilerNode_t* node_1,
                     CompilerNode_t* node_2)
{
    assert (node_1);
    assert (node_2);

    for (int i = 0; i < node_2->amount_children; i++)
        addNode (node_1, node_2->children[i]);

    return 0;
}
// ---------------------------------------------------------------------------------------------------
