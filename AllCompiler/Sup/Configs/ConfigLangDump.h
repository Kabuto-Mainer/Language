#ifndef CONFIG_LANG_DUMP_H
#define CONFIG_LANG_DUMP_H

#include "TypesOfType.h"
#include "TokenType.h"


// ---------------------------------------------------------------------------------------------------
const int MAX_TOKEN_LEN = 20;
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
const SystemSyntaxWord_t ALL_SYSTEM_WORD[] =
{
    {"extern var",  NODE_TYPE_KEY_WORD,     KEY_DEFINE_VAR},
    {"extern func", NODE_TYPE_KEY_WORD,     KEY_DEFINE_FUNC},
    {"if",          NODE_TYPE_KEY_WORD,     KEY_IF},
    {"else if",     NODE_TYPE_KEY_WORD,     KEY_ELSE_IF},
    {"else",        NODE_TYPE_KEY_WORD,     KEY_ELSE},
    {"return",      NODE_TYPE_KEY_WORD,     KEY_RETURN},
    {"while",       NODE_TYPE_KEY_WORD,     KEY_WHILE},
    {"=",           NODE_TYPE_KEY_WORD,     KEY_ASSIGN},
    {"break",       NODE_TYPE_KEY_WORD,     KEY_BREAK},
    {"in",          NODE_TYPE_KEY_WORD,     KEY_IN},
    {"out",         NODE_TYPE_KEY_WORD,     KEY_OUT},
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
    // {"#",           NODE_TYPE_PUNCT,        PUNCT_H},
    {"&quot;",      NODE_TYPE_PUNCT,        PUNCT_QUOT},
    {"&lt;",        NODE_TYPE_PUNCT,        PUNCT_LEFT_TANG},
    {"&gt;",        NODE_TYPE_PUNCT,        PUNCT_RIGHT_TANG}
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
    {"&lt;&lt;",    NODE_TYPE_OPER,         OPER_COMP_ONLY_LIT},
    {"&gt;&gt;",    NODE_TYPE_OPER,         OPER_COMP_ONLY_BIG},
    {"&lt;=",       NODE_TYPE_OPER,         OPER_COMP_LIT_EQUAL},
    {"&gt;=",       NODE_TYPE_OPER,         OPER_COMP_BIG_EQUAL},
    {"==",          NODE_TYPE_OPER,         OPER_COMP_EQUAL},
    {"!=",          NODE_TYPE_OPER,         OPER_COMP_NOT_EQUAL},
    {"bring adr",   NODE_TYPE_OPER,         OPER_NAME},
    {"unname adr",  NODE_TYPE_OPER,         OPER_UNNAME},
    // {"sqrt",        NODE_TYPE_OPER,         OPER_SQRT}
};
// ---------------------------------------------------------------------------------------------------

#endif /* CONFIG_LANG_DUMP_H */
