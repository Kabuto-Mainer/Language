#include <stdio.h>

#include "../COMMON/comand.h"
#include "../COMMON/support.h"

// int argument_cmd[] = {  1, NUM, // PUSH 0
//                         0, NO,  // ADD 1
//                         0, NO,  // SUB 2
//                         0, NO,  // DIV 3
//                         0, NO,  // MUL 4
//                         0, NO,  // SQRT 5
//                         0, NO,  // OUT 6
//                         0, NO,  // HLT 7
//                         1, REG, // POPR 8
//                         1, REG, // PUSHR 9
//                         0, NO,  // IN 10
//                         1, ADR, // JMP 11
//                         1, ADR, // JB 12
//                         1, ADR, // JBE 13
//                         1, ADR, // JA 14
//                         1, ADR, // JAE 15
//                         1, ADR, // JE 16
//                         1, ADR, // JNE 17
//                         0, NO,  // HACK 18
//                         1, ADR, // CALL 19
//                         0, NO,  // RET 20
//                         1, REG, // PUSHM 21
//                         1, REG, // POPM 22
//                         1, NUM, // PAINT 23
//                         0, NO,  // COLOR 24
//                         0, NO,  // DRAW 25
//                         0, NO   // CTIME 26
// };

// const char name_type_arg[][10] = {"NO",
//                                   "NUM",
//                                   "REG",
//                                   "ADR"
// };

int main(void) {
    // int all_hash[AMOUNT_CMD] = {0};

    FILE* output_stream = fopen_file(NAME_HASH_FILE, "w");

    fprintf(output_stream, "\n// This is auto generation code. Do not change. Use \"make run-h\" to build it\n\n");

    fprintf(output_stream, "#ifndef CMD_HASH_H\n#define CMD_HASH_H\n\n");
    fprintf(output_stream, "#include \"../COMMON/comand.h\"\n\n");

    fprintf(output_stream, "enum INT_HASH {\n");
    fprintf(output_stream, "\n");

    for (size_t i = 0; i < (AMOUNT_CMD); i++) {
        fprintf(output_stream, "    HASH_%s = %d,\n", CHAR_CMD[i], cmd_to_hash(CHAR_CMD[i]));
    }

    fprintf(output_stream, "};\n\n");
    fprintf(output_stream, "#endif // CMD_HASH_H\n");
    fclose_file(output_stream);

    return 0;
}
