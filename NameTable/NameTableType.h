#ifndef NAME_TABLE_TYPE_H
#define NAME_TABLE_TYPE_H

#include "../Support/SupportFunc.h"
#include "../AllTypes.h"
#include "../CompilerType.h"

// ---------------------------------------------------------------------------------------------------
/// @brief
union NameTableValue_t
{
    CompilerVar_t* var;
    CompilerFunc_t* func;
};

/// @brief
typedef char* NameTableName_t;
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
#define assertNameTableName(__name__)   \
    do {                                \
        if (__name__ == NULL)           \
        {                               \
            Log_Error ("Bad name");     \
            assert (__name__);          \
        }                               \
    } while (0)
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
struct NameTableVar_t
{
    size_t hash;
    NameTableName_t name;
    NameTableValue_t value;
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
struct NameTable_t
{
    NameTableVar_t* data;
    size_t size;
    size_t capacity;
};
// ---------------------------------------------------------------------------------------------------


#endif // NAME_TABLE_TYPE_H
