#//.PHONY: clean run build gitcom

# Запуск всей программы
all: hash-cmd assembler processor test disassembler

# Флаги компиляции
flags = -D_DEBUG -Werror -ggdb3 -lSDL2 -std=c++17 -O3 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

# ----------------------------------------------------------------------------------------------------------
# Общие .cpp файлы
common_cpp = COMMON/support.cpp

# Общие файлы
common_all = COMMON/color.h COMMON/comand.h COMMON/const.h COMMON/Makefile COMMON/support.h COMMON/cmd-hash.h $(common_cpp)
# ----------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------
# Файлы .cpp stack
stack_cpp = PROCESSOR/stack.cpp

# Файлы stack
stack_all = PROCESSOR/stack_define.h PROCESSOR/stack.h $(stack_cpp)
# ----------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------
# Файлы .cpp для сборки processor
proc_cpp = PROCESSOR/processor.cpp $(stack_cpp) PROCESSOR/OS.cpp PROCESSOR/proc-func.cpp PROCESSOR/display.cpp

# Файлы processor
proc_all = PROCESSOR/processor.h $(stack_all) $(proc_cpp) PROCESSOR/proc-func.h
# ----------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------
# Файлы .cpp assembler
asmb_cpp = ASSEMBLER/assembler.cpp ASSEMBLER/asm-start.cpp ASSEMBLER/asm-sort.cpp

# Файлы assembler
asmb_all = ASSEMBLER/assembler.h $(asmb_cpp) ASSEMBLER/asm-sort.h ASSEMBLER/asm-type.h
# ----------------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------------------------------------------
# Файлы .cpp disassembler
disasmb_cpp = DISASSEMBLER/disassembler.cpp

# Файлы disassembler
disasmb_all = DISASSEMBLER/disassembler.h $(disasmb_cpp)
# ----------------------------------------------------------------------------------------------------------

# Текстовые файлы
text_file = TEXT_FILE/bin_code.bin TEXT_FILE/asm_code.asm TEXT_FILE/listing.asm TEXT_FILE/disasmb-text.asm


# ----------------------------------------------------------------------------------------------------------
# Файлы .cpp для TEST
test_cpp = TEST/test-func.cpp ASSEMBLER/asm-sort.cpp ASSEMBLER/assembler.cpp

# Файлы для TEST
test_all = TEST/
# ----------------------------------------------------------------------------------------------------------

comand_cpp = PROCESSOR/processor.cpp PROCESSOR/proc-func.cpp PROCESSOR/stack.cpp PROCESSOR/display.cpp


# ----------------------------------------------------------------------------------------------------------
# Компиляция processor
processor:
	@echo -------------------------------------------------------------------------
	g++ $(proc_cpp) $(common_cpp) $(flags) -lX11 -o BIN/processor.exe

# Компиляция assembler
assembler:
	@echo -------------------------------------------------------------------------
	g++ $(asmb_cpp) $(comand_cpp) $(common_cpp)  $(flags) -o BIN/assembler.exe

# Компиляция hash-cmd
hash-cmd:
	@echo -------------------------------------------------------------------------
	g++ ASSEMBLER/cmd-hash-create.cpp $(comand_cpp) $(common_cpp)  $(flags) -o BIN/hash-cmd.exe

# Компиляция test
test:
	@echo -------------------------------------------------------------------------
	g++ $(test_cpp) $(comand_cpp) $(common_cpp) $(flags) -o BIN/test.exe

# Компиляция disassembler
disassembler:
	@echo -------------------------------------------------------------------------
	g++ $(disasmb_cpp) $(common_cpp) $(comand_cpp) $(flags) -o BIN/disassembler.exe
# ----------------------------------------------------------------------------------------------------------

# Компиляция list
list:
	@echo -------------------------------------------------------------------------
	g++ LIST/list-func.cpp LIST/test-list.cpp LIST/list-dump.cpp LIST/list-user-f.cpp LIST/list-ver.cpp COMMON/support.cpp $(flags) -D'GLOBAL=ON' -o BIN/list.exe

list-speed:
	@echo Compilation list.exe
	g++ LIST/list-func.cpp LIST/list-user-f.cpp LIST/list-dump.cpp LIST/list-ver.cpp LIST/test-list.cpp  COMMON/support.cpp $(flags) -D'GLOBAL=ON' -D'VERIFIER=OFF' -o BIN/list.exe


# Компиляция list-common
list-c:
	@echo -------------------------------------------------------------------------
	g++ LIST-COMMON/list-func.cpp LIST-COMMON/list-test.cpp COMMON/support.cpp $(flags) -o BIN/list-c.exe

list-c-speed:
	@echo -------------------------------------------------------------------------
	g++ LIST-COMMON/list-func.cpp LIST-COMMON/list-test.cpp COMMON/support.cpp $(flags) -DVERIFIER=OFF -o BIN/list-c.exe


# ----------------------------------------------------------------------------------------------------------
# Запуск assembler
run-a:
	@echo -------------------------------------------------------------------------
	./BIN/assembler.exe $(arg)

# Запуск processor
run-p:
	@echo -------------------------------------------------------------------------
	./BIN/processor.exe $(arg)

# Запуск disassembler
run-d:
	@echo -------------------------------------------------------------------------
	./BIN/disassembler.exe

# Запуск hash-cmd-create
run-h:
	@echo -------------------------------------------------------------------------
	./BIN/hash-cmd.exe

# Запуск test
run-t:
	@echo -------------------------------------------------------------------------
	./BIN/test.exe
# ----------------------------------------------------------------------------------------------------------

# Запуск list
run-l:
	@echo -------------------------------------------------------------------------
	./BIN/list.exe
# ----------------------------------------------------------------------------------------------------------

# Запуск list common
run-lc:
	@echo -------------------------------------------------------------------------
	./BIN/list-c.exe
# ----------------------------------------------------------------------------------------------------------


# Git commit
gitcom:
	@(git add ASSEMBLER/ BIN/ COMMON/ DISASSEMBLER/ PROCESSOR/ Makefile myasm/ TEST/ LIST/ LIST-COMMON/ TREE/)
	@echo -- Add file to git commit ---
	@(git commit -m "$(m)")
	@echo --- Git committing ---
	@(git push)
	@echo --- Git push ---

