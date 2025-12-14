# ============================================================================
# Общий makefile
# ============================================================================

.PHONY: all clean run build gitcom lang assembler processor hash-cmd test disassembler list list-c

# ============================================================================
# Пути для всех файлов
# ============================================================================

BIN_DIR = Bin
SYSTEM_DIR = System

# ============================================================================
# Флаги компиляции
# ============================================================================

FLAGS_COMMON = -D_DEBUG -ggdb3 -std=c++17 -O3 -Wall -Wextra -Weffc++ \
               -Waggressive-loop-optimizations -Wc++14-compat \
               -Wmissing-declarations -Wcast-align -Wcast-qual \
               -Wchar-subscripts -Wconditionally-supported -Wconversion \
               -Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
               -Wformat-nonliteral -Wformat-security -Wformat-signedness \
               -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor \
               -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith \
               -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion \
               -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 \
               -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
               -Wsuggest-final-types -Wsuggest-override -Wswitch-default \
               -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code \
               -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix \
               -Wno-missing-field-initializers -Wno-narrowing \
               -Wno-old-style-cast -Wno-varargs -Wstack-protector \
               -fcheck-new -fsized-deallocation -fstack-protector \
               -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer \
               -pie -fPIE -Werror=vla

FLAGS_SANITIZE = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

# Сборные флаги
FLAGS = $(FLAGS_COMMON) -Werror
LDFLAGS = $(FLAGS_SANITIZE)
FLAGS_NO_SANITIZE = $(FLAGS_COMMON)
FLAGS_RELEASE = $(FLAGS_COMMON) -O3 -DNDEBUG

# ============================================================================
# Файлы для виртуальной машины
# ============================================================================

# Общие файлы
PROC_COMMON_CPP = Proc/COMMON/support.cpp
PROC_COMMON_ALL = Proc/COMMON/color.h Proc/COMMON/comand.h Proc/COMMON/const.h \
			 	  Proc/COMMON/support.h Proc/COMMON/cmd-hash.h $(PROC_COMMON_CPP)

# Файлы стека процессора
PROC_STACK_CPP = Proc/PROCESSOR/stack.cpp
PROC_STACK_ALL = Proc/PROCESSOR/stack_define.h Proc/PROCESSOR/stack.h

# Файлы самого процессора
PROC_PROC_CPP = Proc/PROCESSOR/processor.cpp $(PROC_STACK_CPP) \
				Proc/PROCESSOR/OS.cpp Proc/PROCESSOR/proc-func.cpp \
				Proc/PROCESSOR/display.cpp
PROC_PROC_H = Proc/PROCESSOR/processor.h $(PROC_STACK_ALL) $(PROC_PROC_CPP) \
			  Proc/PROCESSOR/proc-func.h

# Файлы ассемблера
PROC_ASM_CPP = Proc/ASSEMBLER/assembler.cpp Proc/ASSEMBLER/asm-start.cpp \
			   Proc/ASSEMBLER/asm-sort.cpp
PROC_ASM_H = Proc/ASSEMBLER/assembler.h Proc/ASSEMBLER/asm-sort.h Proc/ASSEMBLER/asm-type.h
PROC_ASM_DOP_CPP = Proc/PROCESSOR/processor.cpp Proc/PROCESSOR/proc-func.cpp \
				   Proc/PROCESSOR/stack.cpp Proc/PROCESSOR/display.cpp


# Файлы компилятора (lang)
DUMP_CPP = 		AllCompiler/Sup/DumpCode/DumpFunc.cpp
TABLE_CPP = 	AllCompiler/Struct/NameTable/NameTableFunc.cpp
PARSER_CPP = 	AllCompiler/Front/Parser/ParserFunc.cpp \
				AllCompiler/Front/Parser/WriteTreeFunc.cpp
SUPPORT_CPP = 	AllCompiler/Sup/Support/Support.cpp \
				AllCompiler/Sup/Common/NodeFunc.cpp
TOKEN_CPP = 	AllCompiler/Front/Token/TokenFunc.cpp
VECTOR_CPP = 	AllCompiler/Struct/Vector/Vector.cpp
BACK_CPP = 		AllCompiler/Back/ParseTree.cpp \
				AllCompiler/Back/CompilerFunc.cpp \
				AllCompiler/Back/Stack.cpp

# Инклюд-директории для компилятора
INCLUDES_LANG = -IAllCompiler/Sup/Common/ -IAllCompiler/Struct/NameTable/ \
				-IAllCompiler/Front/Parser/ -IAllCompiler/Sup/Support/ \
				-IAllCompiler/Front/Token/ -IAllCompiler/Struct/Vector/ \
                -IAllCompiler/Sup/DumpCode/ -IAllCompiler/Sup/Configs/ \
				-IAllCompiler/Back/

# ============================================================================
# Цели сборки
# ============================================================================

all: hash-cmd assembler processor lang

# ----------------------------------------------------------------------------
# Компилятор (исходный код → AST/asm)
# ----------------------------------------------------------------------------
lang:
	@echo "========================================================================"
	@echo "Сборка компилятора lang..."
	mkdir -p $(BIN_DIR)
	g++ $(INCLUDES_LANG) $(FLAGS) AllCompiler/main.cpp $(VECTOR_CPP) \
		$(TOKEN_CPP) $(SUPPORT_CPP) $(TABLE_CPP) $(DUMP_CPP) $(PARSER_CPP) \
		$(BACK_CPP) $(LDFLAGS) -o $(BIN_DIR)/lang
	@echo "✓ Компилятор собран: $(SYSTEM_DIR)/lang"

# ----------------------------------------------------------------------------
# Ассемблер (asm → bin)
# ----------------------------------------------------------------------------
assembler:
	@echo "========================================================================"
	@echo "Сборка ассемблера..."
	@mkdir -p $(BIN_DIR)
	g++ $(FLAGS) $(PROC_ASM_CPP) $(PROC_COMMON_CPP) $(PROC_ASM_DOP_CPP) -lSDL2 $(LDFLAGS) -o $(BIN_DIR)/assembler
	@echo "✓ Ассемблер собран: $(BIN_DIR)/assembler"

# ----------------------------------------------------------------------------
# Процессор (bin → выполнение)
# ----------------------------------------------------------------------------
processor:
	@echo "========================================================================"
	@echo "Сборка процессора..."
	@mkdir -p $(BIN_DIR)
	g++ $(FLAGS) $(PROC_PROC_CPP) $(PROC_COMMON_CPP) -lSDL2 $(LDFLAGS) -o $(BIN_DIR)/processor
	@echo "✓ Процессор собран: $(BIN_DIR)/processor"

# ----------------------------------------------------------------------------
# Генератор хеш-команд
# ----------------------------------------------------------------------------
hash-cmd:
	@echo "========================================================================"
	@echo "Сборка генератора хеш-команд..."
	@mkdir -p $(BIN_DIR)
	g++ $(FLAGS) $(PROC_ASM_DOP_CPP) $(PROC_COMMON_CPP) -lSDL2 $(LDFLAGS) -o $(BIN_DIR)/hash-cmd
	@echo "✓ Генератор хеш-команд собран: $(BIN_DIR)/hash-cmd"

# ============================================================================
# Цели запуска
# ============================================================================

# Запуск компилятора
run-l:
	@echo "========================================================================"
	@echo "Запуск компилятора..."
	./$(BIN_DIR)/lang

# Запуск ассемблера
run-a:
	@echo "========================================================================"
	@echo "Запуск ассемблера..."
	./$(BIN_DIR)/assembler $(arg)

# Запуск процессора
run-p:
	@echo "========================================================================"
	@echo "Запуск процессора..."
	./$(BIN_DIR)/processor $(arg)

# ============================================================================
# Утилиты
# ============================================================================

# Очистка
clean:
	@echo "Очистка бинарных файлов..."
	rm -rf $(BIN_DIR)/*.exe $(SYSTEM_DIR)/*.exe
	@echo "✓ Очистка завершена"

# Git commit
gitcom:
	@echo "========================================================================"
	@echo "Git: добавление файлов..."
	git add ASSEMBLER/ BIN/ COMMON/ DISASSEMBLER/ PROCESSOR/ Makefile myasm/ TEST/ LIST/ LIST-COMMON/ TREE/ Sup/ Struct/ Front/ Back/ System/ main.cpp
	@echo "Git: коммит с сообщением '$(m)'..."
	git commit -m "$(m)"
	@echo "Git: отправка на сервер..."
	git push
	@echo "✓ Git операции завершены"

# Сборка виртуальной машины
virtbox: hash-cmd assembler processor
	@echo "========================================================================"
	@echo "✓ Виртуальная машина собрана:"
	@echo "  - hash-cmd: генератор хешей"
	@echo "  - assembler: ассемблер"
	@echo "  - processor: процессор"

# Справка
help:
	@echo "Доступные цели:"
	@echo ""
	@echo "  СБОРКА:"
	@echo "    all           - Собрать всё (lang + assembler + processor)"
	@echo "    virtbox       - Собрать только виртуальную машину (assembler + processor)"
	@echo "    lang          - Собрать компилятор"
	@echo "    assembler     - Собрать ассемблер"
	@echo "    processor     - Собрать процессор"
	@echo ""
	@echo "  ЗАПУСК:"
	@echo "    run-l         - Запустить компилятор"
	@echo "    run-a [arg]   - Запустить ассемблер с аргументами"
	@echo "    run-p [arg]   - Запустить процессор с аргументами"
	@echo ""
	@echo "  УТИЛИТЫ:"
	@echo "    clean         - Очистить бинарные файлы"
	@echo "    rebuild       - Полная пересборка"
	@echo "    gitcom m=\"msg\" - Git commit с сообщением"
	@echo "    help          - Показать эту справку"

