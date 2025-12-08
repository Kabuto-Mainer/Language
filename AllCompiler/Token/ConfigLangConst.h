#ifndef CONFIG_LANG_CONST_H
#define CONFIG_LANG_CONST_H

#include "TypesOfType.h"
#include "TokenType.h"


// ---------------------------------------------------------------------------------------------------
const int MAX_TOKEN_LEN = 20;
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
const SystemSyntaxWord_t ALL_SYSTEM_WORD[] =
{
    {"alloc",       NODE_TYPE_KEY_WORD,     KEY_DEFINE_VAR},
    {"server",      NODE_TYPE_KEY_WORD,     KEY_DEFINE_FUNC},
    {"trig",        NODE_TYPE_KEY_WORD,     KEY_IF},
    {"nexttrig",    NODE_TYPE_KEY_WORD,     KEY_ELSE_IF},
    {"default",     NODE_TYPE_KEY_WORD,     KEY_ELSE},
    {"push",        NODE_TYPE_KEY_WORD,     KEY_RETURN},
    {"loop",        NODE_TYPE_KEY_WORD,     KEY_WHILE},
    {"=",           NODE_TYPE_KEY_WORD,     KEY_ASSIGN}
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
const SystemSyntaxWord_t ALL_PUNCT_WORD[] =
{
    {",",           NODE_TYPE_PUNCT,        PUNCT_COMMA},
    {"(",           NODE_TYPE_PUNCT,        PUNCT_LEFT_ROUND},
    {")",           NODE_TYPE_PUNCT,        PUNCT_RIGHT_ROUND},
    // {"{",           NODE_TYPE_PUNCT,        PUNCT_LEFT_FIGURE},
    // {"}",           NODE_TYPE_PUNCT,        PUNCT_RIGHT_FIGURE},
    // {";",          NODE_TYPE_PUNCT,         PUNCT_END_TREE_STR},
    {"\\",          NODE_TYPE_PUNCT,        PUNCT_NEXT_STR},
    {"\n",          NODE_TYPE_PUNCT,        PUNCT_END_STR},
    {"@",           NODE_TYPE_PUNCT,        PUNCT_DOG},
    {"#",           NODE_TYPE_PUNCT,        PUNCT_H},
    {"\"",          NODE_TYPE_PUNCT,        PUNCT_QUOT},
    {"<",           NODE_TYPE_PUNCT,        PUNCT_LEFT_TANG},
    {">",           NODE_TYPE_PUNCT,        PUNCT_RIGHT_TANG}
};
// ---------------------------------------------------------------------------------------------------

// // ---------------------------------------------------------------------------------------------------
// const char PUNCT_SYMBOLS[] =
// {
//     ',', '(', ')', '\\', '\n', '@', '#', '\"', '<', '>'
// };
// // ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
const SystemSyntaxWord_t ALL_OPER_WORD[] =
{
    {"+",           NODE_TYPE_OPER,         OPER_ADD},
    {"-",           NODE_TYPE_OPER,         OPER_SUB},
    {"*",           NODE_TYPE_OPER,         OPER_MUL},
    {"/",           NODE_TYPE_OPER,         OPER_DIV},
    {"<<",          NODE_TYPE_OPER,         OPER_COMP_ONLY_LIT},
    {">>",          NODE_TYPE_OPER,         OPER_COMP_ONLY_BIG},
    {"<=",          NODE_TYPE_OPER,         OPER_COMP_LIT_EQUAL},
    {">=",          NODE_TYPE_OPER,         OPER_COMP_BIG_EQUAL},
    {"==",          NODE_TYPE_OPER,         OPER_COMP_EQUAL},
    {"!=",          NODE_TYPE_OPER,         OPER_COMP_NOT_EQUAL},
    {"sqrt",        NODE_TYPE_OPER,         OPER_SQRT}
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
const char OPER_SYMBOLS[] =
{
    '+', '-', '*', '/', '<', '>', '=', '!', 's', 'q', 'r', 't'
};
// ---------------------------------------------------------------------------------------------------

#endif /* CONFIG_LANG_CONST_H */
