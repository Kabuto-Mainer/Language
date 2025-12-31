#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "TypeTable.h"
#include "StringTable.h"
#include "SymbolTable.h"


struct AnalysisContextInf_t
{
    StringTable_t* table_str;
    TypeTable_t* table_type;
    SymbolTable_t* table_global;
    SymbolTable_t* table_func;
};

#endif /* ANALYSIS_H */
