
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "SupportFunc.h"
#include "ParserFunc.h"
#include "TypesOfType.h"
#include "TokenFunc.h"
#include "DumpFunc.h"
#include "NodeFunc.h"

// ---------------------------------------------------------------------------------------------------
#define SYNTAX_ERROR(__context__, __number_error__, __ret_val__)    \
    do {                                                            \
        parserError (__number_error__,__context__);                 \
        EXIT_FUNC ("Syntax Error",__ret_val__);                     \
    } while (0)
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseGlobal (Node_t* start_node, size_t amount,
                        Node_t* global_node, const char* name_file,
                        StringTable_t* table_str)
{
    assert (start_node);
    assert (global_node);
    assert (name_file);
    assert (table_str);

    ParserInf_t inf =
    {
        .str_table = table_str,
    };

    ParserContextInf_t ctx = {};
    inf.ctx = &ctx;

    inf.ctx->capacity = amount;
    inf.ctx->cur_index = 0;
    inf.ctx->name_file = name_file;
    inf.ctx->error = PE_NOT_ERROR;
    inf.ctx->line = 1;
    inf.ctx->node = start_node;

    global_node->type_node = NODE_PROGRAM;
    while (inf.ctx->cur_index < inf.ctx->capacity - 1)
    {
        skipVoid (inf.ctx);
        // tokenOneDump (inf.ctx->node, "GLOBAL");
        if (P_ParseDeclarationFunc (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (P_ParseDeclarationVar (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (P_ParseConditional (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (P_ParseBreak (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (P_ParseContinue (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (P_ParseWhile (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (P_ParseReturn (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (P_ParseCallFunction (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (P_ParseInOut (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (P_ParseAssign (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (inf.ctx->cur_index >= inf.ctx->capacity - 1)
            break;

        SYNTAX_ERROR (inf.ctx, PE_UNKNOWN, PARSER_SYNTAX_ERROR);
    }
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseUnion (ParserInf_t* inf,
                       Node_t* node)
{
    assert (inf);
    assert (node);

    if (!isLeftRound (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_ROUND, PARSER_SYNTAX_ERROR);

    // printf ("UNION\n");
    Node_t* block = inf->ctx->node;
    block->type_node = NODE_BLOCK;

    addNode (node, block);
    nextNode (inf->ctx);

    while (true)
    {
        skipVoid (inf->ctx);
        if (P_ParseDeclarationVar (inf, block) == PARSER_THIS_OK)
            continue;

        // tokenOneDump (inf->ctx->node, "UNION");
        if (P_ParseConditional (inf, block) == PARSER_THIS_OK)
            continue;

        if (P_ParseBreak (inf, block) == PARSER_THIS_OK)
            continue;

        if (P_ParseReturn (inf, block) == PARSER_THIS_OK)
            continue;

        if (P_ParseContinue (inf, block) == PARSER_THIS_OK)
            continue;

        if (P_ParseWhile (inf, block) == PARSER_THIS_OK)
            continue;

        if (P_ParseCallFunction (inf, block) == PARSER_THIS_OK)
            continue;

        if (P_ParseInOut (inf, block) == PARSER_THIS_OK)
            continue;
        // tokenOneDump (inf->ctx->node, "UNION");

        if (P_ParseAssign (inf, block) == PARSER_THIS_OK)
            continue;

        break;
    }
    if (!isRightRound (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_ROUND, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    skipVoid (inf->ctx);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseInOut (ParserInf_t* inf,
                       Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        (inf->ctx->node->val.key != KW_INPUT &&
         inf->ctx->node->val.key != KW_OUTPUT))
        return PARSER_NOT_THIS;

    Node_t* node_key = inf->ctx->node;
    nextNode (inf->ctx);
    skipVoid (inf->ctx);

    if (!isLeftTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    skipVoid (inf->ctx);

    if (P_ParseOrExpression (inf, node_key) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

    if (!isRightTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    if (!isEndChar (inf->ctx->node) && !isRightRound (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_END_CHAR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseAssign (ParserInf_t* inf,
                        Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (isRightRound (inf->ctx->node) ||
        isEndChar (inf->ctx->node))
        return PARSER_NOT_THIS;

    // printf ("Assign\n");
    Node_t* buffer_node = newNode ();
    if (P_ParseOrExpression (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        return PARSER_SYNTAX_ERROR;
    }

    if (inf->ctx->node->type_node != NODE_OPER ||
        inf->ctx->node->val.oper.code != OPER_ASSIGN)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NO_ASSIGN, PARSER_SYNTAX_ERROR);
    }

    Node_t* node_assign = inf->ctx->node;
    node_assign->type_node = NODE_ASSIGN;
    nextNode (inf->ctx);

    addNode (parent, node_assign);
    addChildren (node_assign, buffer_node);
    deleteOneNode (buffer_node);

    if (P_ParseOrExpression (inf, node_assign) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

    if (!isEndChar (inf->ctx->node) && !isRightRound (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_END_CHAR, PARSER_SYNTAX_ERROR);

    return PARSER_THIS_OK;
}


// TODO: Насколько хороша идея создания функции дампа, которая отключается через define для 2 режимов работы: Тестовый и продакшн
// ---------------------------------------------------------------------------------------------------
Status_t P_ParseWhile (ParserInf_t* inf,
                       Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_WHILE)
        return PARSER_NOT_THIS;

    Node_t* node_while = inf->ctx->node;
    node_while->type_node = NODE_WHILE;
    addNode (parent, node_while);
    nextNode (inf->ctx);
    skipVoid (inf->ctx);

    if (!isLeftTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    skipVoid (inf->ctx);

    if (P_ParseOrExpression (inf, node_while) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_NO_CONDITION, PARSER_SYNTAX_ERROR);

    if (!isRightTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    skipVoid (inf->ctx);

    if (P_ParseUnion (inf, node_while) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_NO_UNION, PARSER_SYNTAX_ERROR);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseBreak (ParserInf_t* inf,
                       Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_BREAK)
        return PARSER_NOT_THIS;

    Node_t* node_break = inf->ctx->node;
    addNode (parent, node_break);
    node_break->type_node = NODE_BREAK;

    nextNode (inf->ctx);

    if (!isEndChar (inf->ctx->node) && !isRightRound (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_END_CHAR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseContinue (ParserInf_t* inf,
                          Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_CONTINUE)
        return PARSER_NOT_THIS;

    Node_t* node_continue = inf->ctx->node;
    addNode (parent, node_continue);
    node_continue->type_node = NODE_CONTINUE;

    nextNode (inf->ctx);

    if (!isEndChar (inf->ctx->node) && !isRightRound (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_END_CHAR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseReturn (ParserInf_t* inf,
                        Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_RETURN)
        return PARSER_NOT_THIS;

    Node_t* node_return = inf->ctx->node;
    addNode (parent, node_return);
    node_return->type_node = NODE_RETURN;

    nextNode (inf->ctx);
    if (P_ParseOrExpression (inf, node_return) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);

    if (!isEndChar (inf->ctx->node) && !isRightRound (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_END_CHAR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseDeclarationFunc (ParserInf_t* inf,
                                 Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* decl_node = inf->ctx->node;
    if (decl_node->type_node != NODE_KEY ||
        decl_node->val.key != KW_FUNCTION)
        return PARSER_NOT_THIS;

    decl_node->type_node = NODE_FUNC_DECL;
    nextNode (inf->ctx);

    Node_t* func_id = inf->ctx->node;
    if (func_id->type_node != NODE_ID_RAW)
        SYNTAX_ERROR (inf->ctx, PE_INCORRECT_NAME, PARSER_SYNTAX_ERROR);

    func_id->type_node = NODE_ID_TREE;
    func_id->val.id_tree.name = func_id->val.id_raw.name;

    addNode (parent, decl_node);
    addNode (decl_node, func_id);
    nextNode (inf->ctx);

    if (!isLeftTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    while (true)
    {
        // tokenOneDump (inf->ctx->node, "PARAM");
        Node_t* param = inf->ctx->node;
        if (param->type_node != NODE_ID_RAW)
            SYNTAX_ERROR (inf->ctx, PE_NO_PARAM, PARSER_SYNTAX_ERROR);

        nextNode (inf->ctx);
        if (inf->ctx->node->type_node != NODE_PUNCT &&
            inf->ctx->node->val.punct != PUNCT_COLON)
            SYNTAX_ERROR (inf->ctx, PE_NO_COLON, PARSER_SYNTAX_ERROR);

        nextNode (inf->ctx);
        TreeType_t type_param = P_ParseType (inf);
        if (type_param.srt.name == NULL)
            SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

        param->type_node = NODE_ID_TREE;
        param->val.id_tree.name = param->val.id_raw.name;
        param->val.id_tree.type = type_param;
        addNode (decl_node, param);

        if (isComma (inf->ctx->node))
        {
            nextNode (inf->ctx);
            continue;
        }
        break;
    }
    if (!isRightTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    skipVoid (inf->ctx);

    if (!isArrow (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_TYPE, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    TreeType_t type = P_ParseType (inf);
    if (type.srt.name == NULL)
        SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

    if (inf->ctx->node->type_node != NODE_PUNCT ||
        inf->ctx->node->val.punct != PUNCT_LPAREN)
        return PARSER_THIS_OK;

    if (P_ParseUnion (inf, decl_node) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseDeclarationVar (ParserInf_t* inf,
                                Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_VARIABLE)
        return PARSER_NOT_THIS;

    // tokenOneDump (inf->ctx->node, "Before variable");
    Node_t* define_node = inf->ctx->node;
    define_node->type_node = NODE_VAR_DECL;

    addNode (parent, define_node);
    nextNode (inf->ctx);
    // skipVoid (inf->ctx);

    TreeType_t type_var = P_ParseType (inf);
    if (type_var.srt.name == NULL)
        SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

    while (true)
    {
        Node_t* var_id = inf->ctx->node;
        if (var_id->type_node != NODE_ID_RAW)
            SYNTAX_ERROR (inf->ctx, PE_INCORRECT_NAME, PARSER_SYNTAX_ERROR);

        var_id->type_node = NODE_ID_TREE;
        var_id->val.id_tree.name = var_id->val.id_raw.name;
        var_id->val.id_tree.type = type_var;

        nextNode (inf->ctx);
        // tokenOneDump (inf->ctx->node, "hhh");
        // skipVoid (inf->ctx);

        Node_t* next_node = inf->ctx->node;
        if (next_node->type_node == NODE_OPER &&
            next_node->val.oper.code == OPER_ASSIGN)
        {
            addNode (next_node, var_id);
            addNode (define_node, next_node);

            nextNode (inf->ctx);
            if (P_ParseOrExpression (inf, next_node) != PARSER_THIS_OK)
                SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);
        }
        else
        {
            addNode (define_node, var_id);
        }

        if (isComma (inf->ctx->node))
        {
            nextNode (inf->ctx);
            continue;
        }
        break;
    }

    if (!isEndChar (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_END_CHAR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
TreeType_t P_ParseType (ParserInf_t* inf)
{
    assert (inf);

    TreeType_t type = {};
    int ptr_lvl = 0;
    while (inf->ctx->node->type_node == NODE_OPER &&
           inf->ctx->node->val.oper.code == OPER_ADDR)
    {
        ptr_lvl++;
        nextNode (inf->ctx);
    }
    type.srt.ptr_lvl = ptr_lvl;

    if (inf->ctx->node->type_node != NODE_ID_RAW)
        SYNTAX_ERROR (inf->ctx, PE_INCORRECT_TYPE, {});

    StringEntry_t* name_type = inf->ctx->node->val.id_raw.name;
    type.srt.name = name_type;
    type.srt.array_size = 0;
    // printf ("Name Type: %s\n", name_type->string);

    nextNode (inf->ctx);

    if (isLeftQuad (inf->ctx->node))
    {
        nextNode (inf->ctx);
        if (inf->ctx->node->type_node != NODE_NUM)
            SYNTAX_ERROR (inf->ctx, PE_INCORRECT_ARRAY_SIZE, {});

        int amount = inf->ctx->node->val.num;
        if (amount <= 0)
            SYNTAX_ERROR (inf->ctx, PE_INCORRECT_ARRAY_SIZE, {});

        nextNode (inf->ctx);
        if (!isRightQuad (inf->ctx->node))
            SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_BRACKET, {});

        nextNode (inf->ctx);
        type.srt.array_size = amount;
    }

    return type;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseOrExpression (ParserInf_t* inf,
                              Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode ();
    if (P_ParseAndExpression (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           inf->ctx->node->val.oper.code == OPER_OR)
    {
        Node_t* logic_or = inf->ctx->node;
        addChildren (logic_or, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseAndExpression (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
        }

        addChildren (logic_or, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode();
        addNode (buffer_node, logic_or);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseAndExpression (ParserInf_t* inf,
                               Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode ();
    if (P_ParseEqualExpression (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           inf->ctx->node->val.oper.code == OPER_AND)
    {
        Node_t* logic_and = inf->ctx->node;
        addChildren (logic_and, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseEqualExpression (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
        }

        addChildren (logic_and, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode ();
        addNode (buffer_node, logic_and);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseEqualExpression (ParserInf_t* inf,
                                 Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode ();
    if (P_ParseCompareExpression (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           (inf->ctx->node->val.oper.code == OPER_E ||
            inf->ctx->node->val.oper.code == OPER_NE))
    {
        Node_t* eq_node = inf->ctx->node;
        addChildren (eq_node, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseCompareExpression (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
        }
        // tokenOneDump (inf->ctx->node, "FHHFH");

        addChildren (eq_node, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode ();
        addNode (buffer_node, eq_node);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseCompareExpression (ParserInf_t* inf,
                                   Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode();
    if (P_ParseAddSub (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           (inf->ctx->node->val.oper.code == OPER_L ||
            inf->ctx->node->val.oper.code == OPER_B ||
            inf->ctx->node->val.oper.code == OPER_LE ||
            inf->ctx->node->val.oper.code == OPER_BE))
    {
        Node_t* cmp_node = inf->ctx->node;
        addChildren (cmp_node, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseAddSub (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
        }

        addChildren (cmp_node, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode ();
        addNode (buffer_node, cmp_node);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseAddSub (ParserInf_t* inf,
                        Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode();
    if (P_ParseMulDiv (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           (inf->ctx->node->val.oper.code == OPER_ADD ||
            inf->ctx->node->val.oper.code == OPER_SUB))
    {
        Node_t* oper_node = inf->ctx->node;
        addChildren (oper_node, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseMulDiv (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
        }

        addChildren (oper_node, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode ();
        addNode (buffer_node, oper_node);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseMulDiv (ParserInf_t* inf,
                        Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode();
    if (P_ParseUnaryExpression (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           (inf->ctx->node->val.oper.code == OPER_DIV ||
            inf->ctx->node->val.oper.code == OPER_MUL))
    {
        Node_t* oper_node = inf->ctx->node;
        addChildren (oper_node, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseUnaryExpression (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);
        }

        addChildren (oper_node, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode ();
        addNode (buffer_node, oper_node);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseUnaryExpression (ParserInf_t* inf,
                                 Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node == NODE_OPER &&
        (inf->ctx->node->val.oper.code == OPER_NOT ||
         inf->ctx->node->val.oper.code == OPER_ADDR ||
         inf->ctx->node->val.oper.code == OPER_DEREF))
    {
        Node_t* unary_node = inf->ctx->node;
        nextNode (inf->ctx);
        addNode (parent, unary_node);

        if (P_ParseUnaryExpression (inf, unary_node) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);

        return PARSER_THIS_OK;
    }
    return P_ParsePrimaryExpression (inf, parent);
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParsePrimaryExpression (ParserInf_t* inf,
                                   Node_t* parent)
{
    assert (inf);
    assert (parent);

    // printf ("PARSER SOMETHING\n");
    // tokenOneDump (inf->ctx->node, "In Primary");
    if (isLeftRound (inf->ctx->node))
    {
        nextNode (inf->ctx);
        if (P_ParseOrExpression (inf, parent) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);

        // skipVoid (inf);
        if (!isRightRound (inf->ctx->node))
            SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_ROUND, PARSER_SYNTAX_ERROR);

        nextNode (inf->ctx);
        return PARSER_THIS_OK;
    }
    if (inf->ctx->node->type_node == NODE_KEY &&
        inf->ctx->node->val.key == KW_CALL)
    {
        return P_ParseCallFunction (inf, parent);
    }
    else if (inf->ctx->node->type_node == NODE_NUM)
    {
        // tokenOneDump (inf->ctx->node, "000");
        addNode (parent, inf->ctx->node);
        nextNode (inf->ctx);
        return PARSER_THIS_OK;
    }
    else if (inf->ctx->node->type_node == NODE_ID_RAW)
    {
        return P_ParseVariable (inf, parent);
    }

    return PARSER_SYNTAX_ERROR;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseCallFunction (ParserInf_t* inf,
                              Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_CALL)
        return PARSER_NOT_THIS;

    Node_t* node_call = inf->ctx->node;
    node_call->type_node = NODE_CALL;
    addNode (parent, node_call);
    nextNode (inf->ctx);

    if (inf->ctx->node->type_node != NODE_ID_RAW)
        SYNTAX_ERROR (inf->ctx, PE_INCORRECT_NAME, PARSER_SYNTAX_ERROR);

    Node_t* node_func = inf->ctx->node;
    node_func->type_node = NODE_ID_TREE;
    node_func->val.id_tree.name = node_func->val.id_raw.name;
    addNode (node_call, node_func);
    nextNode (inf->ctx);

    if (!isLeftTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    while (true)
    {
        if (P_ParseOrExpression (inf, node_call) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);

        if (isComma (inf->ctx->node))
        {
            nextNode (inf->ctx);
            skipVoid (inf->ctx);
            continue;
        }
        break;
    }

    if (!isRightTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseVariable (ParserInf_t* inf,
                          Node_t* parent)
{
    assert (inf);
    assert (parent);

    // printf ("PARSE VARIABLE\n");
    if (inf->ctx->node->type_node != NODE_ID_RAW)
        return PARSER_NOT_THIS;

    Node_t* node_var = inf->ctx->node;
    node_var->type_node = NODE_ID_TREE;
    node_var->val.id_tree.name = node_var->val.id_raw.name;
    nextNode (inf->ctx);

    Node_t* current_node = node_var;

    while (inf->ctx->node->type_node == NODE_PUNCT &&
           (inf->ctx->node->val.punct == PUNCT_LBRACKET ||
            inf->ctx->node->val.punct == PUNCT_DOT))
    {
        if (inf->ctx->node->val.punct == PUNCT_LBRACKET)
        {
            Node_t* node_index = inf->ctx->node;
            node_index->type_node = NODE_INDEX;
            addNode (node_index, current_node);
            nextNode (inf->ctx);

            if (P_ParseAddSub (inf, node_index) != PARSER_THIS_OK)
                SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);

            if (inf->ctx->node->type_node != NODE_PUNCT ||
                inf->ctx->node->val.punct != PUNCT_RBRACKET)
                SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_BRACKET, PARSER_SYNTAX_ERROR);

            nextNode (inf->ctx);
            current_node = node_index;
        }
        else if (inf->ctx->node->val.punct == PUNCT_DOT)
        {
            Node_t* node_field = inf->ctx->node;
            node_field->type_node = NODE_FIELD;
            nextNode (inf->ctx);

            if (inf->ctx->node->type_node != NODE_ID_RAW)
                SYNTAX_ERROR (inf->ctx, PE_INCORRECT_NAME, PARSER_SYNTAX_ERROR);

            Node_t* field = inf->ctx->node;
            field->type_node = NODE_ID_TREE;
            field->val.id_tree.name = field->val.id_raw.name;
            nextNode (inf->ctx);

            addNode (node_field, current_node);
            addNode (node_field, field);
            current_node = node_field;
        }
    }

    addNode (parent, current_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseConditional (ParserInf_t* inf,
                             Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_IF)
        return PARSER_NOT_THIS;

    // printf ("Condition\n");
    Node_t* node_if = inf->ctx->node;
    node_if->type_node = NODE_IF;
    nextNode (inf->ctx);

    if (!isLeftTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_TANG, PARSER_SYNTAX_ERROR);

    /* Т.к. создавать узлы нежелательно, поэтому используем все, что есть */
    Node_t* node_list = inf->ctx->node;
    node_list->type_node = NODE_COND_LIST;

    addNode (parent, node_list);
    addNode (node_list, node_if);
    nextNode (inf->ctx);

    // tokenOneDump (inf->ctx->node, "IIIII");
    if (P_ParseOrExpression (inf, node_if) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);

    if (!isRightTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    if (P_ParseUnion (inf, node_if) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    while (inf->ctx->node->type_node == NODE_KEY &&
           inf->ctx->node->val.key == KW_ELIF)
    {
        Node_t* node_elif = inf->ctx->node;
        node_elif->type_node = NODE_ELSE_IF;

        nextNode (inf->ctx);
        if (!isLeftTang (inf->ctx->node))
            SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_TANG, PARSER_SYNTAX_ERROR);

        nextNode (inf->ctx);
        if (P_ParseOrExpression (inf, node_elif) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf->ctx, PE_NO_EXPRESSION, PARSER_SYNTAX_ERROR);

        if (!isRightTang (inf->ctx->node))
            SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

        nextNode (inf->ctx);
        skipVoid (inf->ctx);
        if (P_ParseUnion (inf, node_elif) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

        addNode (node_list, node_elif);
        skipVoid (inf->ctx);
    }
    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_ELSE)
        return PARSER_THIS_OK;

    Node_t* node_else = inf->ctx->node;
    node_else->type_node = NODE_ELSE;
    nextNode (inf->ctx);
    skipVoid (inf->ctx);

    if (P_ParseUnion (inf, node_else) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

    addNode (node_list, node_else);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция проверки узла на соответствие с типом пунктуации и значением
 * \param [in] node Узел, который проверяют
 * \param [in] val ЗначениеЮ которое должно быть у узла
 * \return 1 - если совпадает, иначе 0
 */
int checkPunctNode (Node_t* node, int val)
{
    assert (node);

    if (node->type_node == NODE_PUNCT &&
        node->val.punct == val)
        return 1;

    return 0;
}

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция смены текущего узла в контексте
 @param [in] inf Указатель на структуру контекста
*/
int nextNode (ParserContextInf_t* inf)
{
    assert (inf);

    if (inf->cur_index == inf->capacity - 1)
        return 1;
    // tokenOneDump (inf->node, "Bla Bla Bla");

    if (inf->node->type_node == NODE_PUNCT &&
        inf->node->val.punct == PUNCT_ENDSTRING)
        inf->line++;

    inf->node = inf->node + 1;
    inf->cur_index++;
    return 0;
}

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция пропуска всех разделяющий символов (\n и \)
 * \param [in] inf Указатель на структуру контекста
*/
int skipVoid (ParserContextInf_t* inf)
{
    assert (inf);

    while (inf->node->type_node == NODE_PUNCT &&
           inf->node->val.punct == PUNCT_ENDSTRING)
    {
        if (inf->cur_index == inf->capacity - 1)
            return 1;

        inf->node = inf->node + 1;
        inf->cur_index++;
        inf->line++;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция выдачи следующего неконечного контекстного токена
 @param [in] inf Указатель на структуру контекста
 @return Узел
*/
Node_t* getNextNotEndNode (ParserContextInf_t* inf)
{
    assert (inf);
    Node_t* node = inf->node + 1;

    while (inf->node->type_node == NODE_PUNCT &&
           inf->node->val.punct == PUNCT_ENDSTRING)
        node = node + 1;

    return node;
}

int parserError (ParserError_t error, ParserContextInf_t* inf)
{
    assert (inf);

    if (inf->error != PE_NOT_ERROR)
        return 0;

    inf->error = error;

    printf ("\033[1;31mSyntax Error\033[0m in \033[1m%s:%d\033[0m\n",
           inf->name_file, inf->line);
    printf ("  ");

    switch (error)
    {
        case PE_UNKNOWN:
            printf ("Неожиданная конструкция. Ожидалось объявление функции, ");
            printf ("переменной или оператора.");
            break;

        case PE_ERROR:
            printf ("Синтаксическая ошибка в выражении или операторе.");
            break;

        case PE_NO_ASSIGN:
            printf ("Ожидался оператор присваивания '='.");
            printf ("\n  Подсказка: проверьте правильность записи выражения.");
            break;

        case PE_NO_CONDITION:
            printf ("Отсутствует условие в операторе управления.");
            printf ("\n  Подсказка: после 'if' или 'while' должно идти условие: < выражение >");
            break;

        case PE_NO_UNION:
            printf ("Ожидался блок операторов.");
            printf ("\n  Подсказка: тело функции, цикла или условия должно быть заключено в ( ... )");
            break;

        case PE_NO_PARAM:
            printf ("Ожидалось имя параметра функции.");
            printf ("\n  Подсказка: параметры функции: function имя < param1: тип, param2: тип >");
            break;

        case PE_NO_COLON:
            printf ("Отсутствует ':' между именем параметра и его типом.");
            printf ("\n  Подсказка: правильный синтаксис: param_name: int");
            break;

        case PE_NO_TYPE:
            printf ("Не указан тип переменной или возвращаемое значение функции.");
            printf ("\n  Подсказка: после '->' должен идти тип возвращаемого значения.");
            break;

        case PE_NO_EXPRESSION:
            printf ("Ожидалось выражение.");
            printf ("\n  Подсказка: выражение может быть переменной, числом, операцией или вызовом функции.");
            break;

        case PE_NO_END_CHAR:
            printf ("Отсутствует символ окончания оператора (';' или новая строка).");
            printf ("\n  Подсказка: большинство операторов должны заканчиваться символом ';'");
            break;

        case PE_INCORRECT_NAME:
            printf ("Некорректное имя идентификатора.");
            printf ("\n  Подсказка: имя должно начинаться с буквы и содержать только буквы, цифры и '_'");
            break;

        case PE_INCORRECT_TYPE:
            printf ("Неизвестный или некорректный тип данных.");
            printf ("\n  Подсказка: проверьте, объявлен ли этот тип. Базовые типы: int, char, void");
            break;

        case PE_INCORRECT_ARRAY_SIZE:
            printf ("Некорректный размер массива.");
            printf ("\n  Подсказка: размер массива должен быть положительным целым числом: [10]");
            break;

        case PE_NOT_LEFT_ROUND:
            printf ("Ожидалась открывающая круглая скобка '('.");
            printf ("\n  Подсказка: блоки кода должны быть заключены в ( ... )");
            break;

        case PE_NOT_RIGHT_ROUND:
            printf ("Ожидалась закрывающая круглая скобка ')'.");
            printf ("\n  Подсказка: проверьте баланс скобок в блоке кода.");
            break;

        case PE_NOT_LEFT_TANG:
            printf ("Ожидалась открывающая угловая скобка '<'.");
            printf ("\n  Подсказка: условия и параметры функций заключаются в < ... >");
            break;

        case PE_NOT_RIGHT_TANG:
            printf ("Ожидалась закрывающая угловая скобка '>'.");
            printf ("\n  Подсказка: проверьте баланс угловых скобок < >.");
            break;

        case PE_NOT_LEFT_BRACKET:
            printf ("Ожидалась открывающая прямоугольная скобка '['.");
            printf ("\n  Подсказка: проверьте баланс прямоугольных скобок.");
            break;

        case PE_NOT_RIGHT_BRACKET:
            printf ("Ожидалась закрывающая прямоугольная скобка ']'.");
            printf ("\n  Подсказка: проверьте баланс прямоугольных скобок.");
            break;

        case PE_NOT_ERROR:
            return 0;

        default:
            printf ("Неизвестный код ошибки парсера: %d", error);
            printf ("\n  Это внутренняя ошибка компилятора.");
    }

    printf ("\n");

    if (inf->node != NULL)
    {
        bool is_token = false;

        switch (inf->node->type_node)
        {
            case NODE_KEY:
            case NODE_ID_RAW:
            case NODE_NUM:
            case NODE_OPER:
            case NODE_PUNCT:
                is_token = true;
                break;

            // Все остальные - это AST-узлы, не токены
            case NODE_ID_TREE:
            case NODE_ID_RESOLVED:
            case NODE_CALL:
            case NODE_INDEX:
            case NODE_FIELD:
            case NODE_IF:
            case NODE_ELSE_IF:
            case NODE_ELSE:
            case NODE_WHILE:
            case NODE_RETURN:
            case NODE_BREAK:
            case NODE_CONTINUE:
            case NODE_ASSIGN:
            case NODE_VAR_DECL:
            case NODE_FUNC_DECL:
            case NODE_STRUCT_DECL:
            case NODE_BLOCK:
            case NODE_PROGRAM:
            case NODE_COND_LIST:
                is_token = false;
                break;

            default:
                break;
        }

        if (is_token)
        {
            tokenOneDump (inf->node);
            printf ("  \033[2mТекущий токен: ");

            switch (inf->node->type_node)
            {
                case NODE_KEY:
                    printf ("ключевое слово");
                    break;

                case NODE_ID_RAW:
                    printf ("идентификатор '%s'",
                        inf->node->val.id_raw.name->string);
                    break;

                case NODE_NUM:
                    printf ("число %d", inf->node->val.num);
                    break;

                case NODE_OPER:
                    printf ("оператор");
                    break;

                case NODE_PUNCT:
                    printf ("пунктуация");
                    break;

                // Эти case'ы нужны для подавления warning
                case NODE_ID_TREE:
                case NODE_ID_RESOLVED:
                case NODE_CALL:
                case NODE_INDEX:
                case NODE_FIELD:
                case NODE_IF:
                case NODE_ELSE_IF:
                case NODE_ELSE:
                case NODE_WHILE:
                case NODE_RETURN:
                case NODE_BREAK:
                case NODE_CONTINUE:
                case NODE_ASSIGN:
                case NODE_VAR_DECL:
                case NODE_FUNC_DECL:
                case NODE_STRUCT_DECL:
                case NODE_BLOCK:
                case NODE_PROGRAM:
                case NODE_COND_LIST:
                    break;

                default:
                    break;
            }

            printf ("\033[0m\n");
        }
    }

    return 0;
}
