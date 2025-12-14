#ifndef COMAND_H
#define COMAND_H

#include "cmd-hash.h"
#include "../PROCESSOR/processor.h"
#include "../PROCESSOR/proc-func.h"
#include "../PROCESSOR/proc-type.h"


// -------------------------------------------------------------------------------------------------------
const char NAME_ASM_FILE[] = "Proc/CODE-FILE/asm-code.asm";
const char NAME_HASH_FILE[] = "Proc/COMMON/cmd-hash.h";
const char NAME_BIN_FILE[] = "Proc/CODE-FILE/bin-code.bin";
const char NAME_TEXT_FILE[] = "Proc/CODE-FILE/listing.asm";
const char NAME_DISASMB_FILE[] = "Proc/CODE-FILE/disasmb-text.asm";
//------------------------------------------------------------------------------------------------
enum J_OPER
{
    OPER_JMP = 0,
    OPER_JA  = 1,
    OPER_JAE = 2,
    OPER_JB  = 3,
    OPER_JBE = 4,
    OPER_JE  = 5,
    OPER_JNE = 6
};
//------------------------------------------------------------------------------------------------
enum P_SUP_NUM
{
    P_OK        = 0,
    P_ERROR     = 1,
    P_END       = 2,
    RAW_TYPE    = 10,
    REG_TYPE    = 11,
    PUSH_TYPE   = 20,
    POP_TYPE    = 21,
    IN_TYPE     = 30,
    OUT_TYPE    = 31,
    CALL_TYPE   = 40,
    RET_TYPE    = 41,
    CLS_TYPE    = 50,
    MACR_TYPE   = 51
};
//------------------------------------------------------------------------------------------------
enum flags_t
{
    F_INP_FILE = 0,
    F_OUT_FILE = 1,
    F_SIZE_DISP = 2,
    F_SOUND = 3
};
//------------------------------------------------------------------------------------------------
const char FLAGS[][20] =
{
    "-InpFile=",
    "-OutFile=",
    "-SizeDisp=",
    "-Sound="
};
//------------------------------------------------------------------------------------------------
enum type_arg_t
{
    NO = 0,
    NUM = 1,
    REG = 2,
    ADR = 3
};
//------------------------------------------------------------------------------------------------
const char CHAR_CMD[][10] = {
                        "PUSH",
                        "ADD",
                        "SUB",
                        "DIV",
                        "MUL",
                        "SQRT",
                        "OUT",
                        "HLT",
                        "POPR",
                        "PUSHR",
                        "IN",
                        "JMP",
                        "JB",
                        "JBE",
                        "JA",
                        "JAE",
                        "JE",
                        "JNE",
                        "HACK",
                        "CALL",
                        "RET",
                        "PUSHM",
                        "POPM",
                        "PAINT",
                        "COLOR",
                        "DRAW",
                        "CTIME",
                        "PAINTXY",
                        "SOUND"
};
// -------------------------------------------------------------------------------------------------------
enum INT_CMD {
    INT_PUSH    = 0,
    INT_ADD     = 1,
    INT_SUB     = 2,
    INT_DIV     = 3,
    INT_MUL     = 4,
    INT_SQRT    = 5,
    INT_OUT     = 6,
    INT_HLT     = 7,
    INT_POPR    = 8,
    INT_PUSHR   = 9,
    INT_IN      = 10,
    INT_JMP     = 11,
    INT_JB      = 12,
    INT_JBE     = 13,
    INT_JA      = 14,
    INT_JAE     = 15,
    INT_JE      = 16,
    INT_JNE     = 17,
    INT_HACK    = 18,
    INT_CALL    = 19,
    INT_RET     = 20,
    INT_PUSHM   = 21,
    INT_POPM    = 22,
    INT_PAINT   = 23,
    INT_COLOR   = 24,
    INT_DRAW    = 25,
    INT_CTIME   = 26,
    INT_PAINTXY = 27,
    INT_SOUND   = 28
};
// -------------------------------------------------------------------------------------------------------
enum INT_REG{
    AX = 0,
    BX = 1,
    CX = 2,
    DX = 3,
    EX = 4,
    FX = 5,
    GX = 6,
    HX = 7,
    R1 = 8,
    R2 = 9,
    R3 = 10,
    R4 = 11
};
// -------------------------------------------------------------------------------------------------------
const char CHAR_REG[][4] = {
    "AX",
    "BX",
    "CX",
    "DX",
    "EX",
    "FX",
    "GX",
    "HX",
    "R1",
    "R2",
    "R3",
    "R4"
};
//------------------------------------------------------------------------------------------------
struct asm_func_t
{
    INT_HASH hash_cmd;

    int amount_arg;
    type_arg_t type_arg;
    INT_CMD number;

    cpu_func func;
};
//------------------------------------------------------------------------------------------------
const asm_func_t CMD_INF[] =
{
    {HASH_PUSH,    1,  NUM,  INT_PUSH,    *push_func    },
    {HASH_ADD,     0,  NO,   INT_ADD,     *math_func    },
    {HASH_SUB,     0,  NO,   INT_SUB,     *math_func    },
    {HASH_DIV,     0,  NO,   INT_DIV,     *math_func    },
    {HASH_MUL,     0,  NO,   INT_MUL,     *math_func    },
    {HASH_SQRT,    0,  NO,   INT_SQRT,    *sqrt_func    },
    {HASH_OUT,     0,  NO,   INT_OUT,     *in_out_func  },
    {HASH_HLT,     0,  NO,   INT_HLT,     *hlt_func     },
    {HASH_POPR,    1,  REG,  INT_POPR,    *reg_func     },
    {HASH_PUSHR,   1,  REG,  INT_PUSHR,   *reg_func     },
    {HASH_IN,      0,  NO,   INT_IN,      *in_out_func  },
    {HASH_JMP,     1,  ADR,  INT_JMP,     *j_func       },
    {HASH_JB,      1,  ADR,  INT_JB,      *j_func       },
    {HASH_JBE,     1,  ADR,  INT_JBE,     *j_func       },
    {HASH_JA,      1,  ADR,  INT_JA,      *j_func       },
    {HASH_JAE,     1,  ADR,  INT_JAE,     *j_func       },
    {HASH_JE,      1,  ADR,  INT_JE,      *j_func       },
    {HASH_JNE,     1,  ADR,  INT_JNE,     *j_func       },
    {HASH_HACK,    0,  NO,   INT_HACK,    NULL          },
    {HASH_CALL,    1,  ADR,  INT_CALL,    *func_func    },
    {HASH_RET,     0,  NO,   INT_RET,     *func_func    },
    {HASH_PUSHM,   1,  REG,  INT_PUSHM,   *reg_func     },
    {HASH_POPM,    1,  REG,  INT_POPM,    *reg_func     },
    {HASH_PAINT,   1,  NUM,  INT_PAINT,   *vraw_func    },
    {HASH_COLOR,   0,  NO,   INT_COLOR,   *vraw_func    },
    {HASH_DRAW,    0,  NO,   INT_DRAW,    *draw_func    },
    {HASH_CTIME,   0,  NO,   INT_CTIME,   *time_func    },
    {HASH_PAINTXY, 3,  NUM,  INT_PAINTXY, *vraw_func    },
    {HASH_SOUND,   1,  NUM,  INT_SOUND,   *sound_func   }
};
//------------------------------------------------------------------------------------------------


const int AMOUNT_CMD = sizeof(CHAR_CMD) / sizeof(CHAR_CMD[0]);


#endif // COMAND_H
