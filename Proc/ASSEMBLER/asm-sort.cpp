#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "asm-type.h"
#include "asm-sort.h"
#include "../COMMON/comand.h"
#include "../PROCESSOR/proc-type.h"



//------------------------------------------------------------------------------------------------
// TODO +-
int sort_comp_label(const void* struct_1, const void* struct_2)
{
    const label_t* l_1 = (const label_t*) struct_1;
    const label_t* l_2 = (const label_t*) struct_2;

    return (l_1->hash_label > l_2->hash_label) - (l_1->hash_label < l_2->hash_label);
}
//------------------------------------------------------------------------------------------------
int search_comp_label(const void* hash_label, const void* label_struct)
{
    const int* hash = (const int*) hash_label;
    const label_t* label = (const label_t*) label_struct;

    return (*hash > label->hash_label) - (*hash < label->hash_label);
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int sort_comp_var(const void* struct_1, const void* struct_2)
{
    const variable_t* var_1 = (const variable_t*) struct_1;
    const variable_t* var_2 = (const variable_t*) struct_2;

    return (var_1->hash_var > var_2->hash_var) - (var_1->hash_var < var_2->hash_var);
}
//------------------------------------------------------------------------------------------------
int search_comp_var(const void* hash_var, const void* var_struct)
{
    const int* hash = (const int*) hash_var;
    const variable_t* var = (const variable_t*) var_struct;

    return (*hash > var->hash_var) - (*hash < var->hash_var);
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int sort_comp_func(const void* index_1, const void* index_2)
{
    const asm_func_t func_1 = CMD_INF[*((const int*) index_1)];
    const asm_func_t func_2 = CMD_INF[*((const int*) index_2)];

    return (func_1.hash_cmd > func_2.hash_cmd) - (func_1.hash_cmd < func_2.hash_cmd);
}
//------------------------------------------------------------------------------------------------
int search_comp_func(const void* hash_func, const void* index_func)
{
    const int* hash = (const int*) hash_func;
    const int* index = (const int*) index_func;

    return (*hash > CMD_INF[*index].hash_cmd) - (*hash < CMD_INF[*index].hash_cmd);
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int sort_label(asm_struct* asm_data)
{
    assert(asm_data);

    qsort(asm_data->table_label,
          (size_t) asm_data->amount_labels,
          sizeof(asm_data->table_label[0]),
          &sort_comp_label);

    return 0;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int sort_var(asm_struct* asm_data)
{
    assert(asm_data);

    qsort(asm_data->table_var,
          (size_t) asm_data->amount_vars,
          sizeof(asm_data->table_var[0]),
          &sort_comp_var);

    return 0;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int sort_func(asm_struct* asm_data)
{
    assert(asm_data);

    for (int i = 0; i < AMOUNT_CMD; i++){
        asm_data->sort_func[i] = i;
    }

    qsort(asm_data->sort_func,
          (size_t) AMOUNT_CMD,
          sizeof(int),
          &sort_comp_func);

    return 0;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
label_t* search_label(asm_struct* asm_data, int hash_label)
{
    assert(asm_data);

    label_t* return_value = (label_t*) bsearch(&hash_label,
                                       asm_data->table_label,
                                       (size_t) asm_data->amount_labels,
                                       sizeof(asm_data->table_label[0]),
                                       &search_comp_label);

    if (return_value == NULL && asm_data->current_run == 0)
    {
        return (asm_data->table_label);
    }

    else if (return_value == NULL)
    {
        return NULL;
    }

    return return_value;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
variable_t* search_var(asm_struct* asm_data, int hash_var)
{
    assert(asm_data);

    variable_t* return_value = (variable_t*) bsearch((void*) &hash_var,
                                                     asm_data->table_var,
                                                     (size_t) asm_data->amount_vars,
                                                     sizeof(asm_data->table_var[0]),
                                                     &search_comp_var);

    return return_value;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int search_func(int* mass_func, int hash_func)
{
    assert(mass_func);

    int* return_value = (int*) bsearch((void*) &hash_func,
                                              mass_func,
                                              (size_t) AMOUNT_CMD,
                                              sizeof(int),
                                              &search_comp_func);

    if (return_value == NULL)
    {
        return -1;
    }
    return *return_value;
}
//------------------------------------------------------------------------------------------------

