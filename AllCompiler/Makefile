# --------------------------------------------------------------------------------------------
# Флаги компиляции
flags = -D_DEBUG -Werror -ggdb3 -lSDL2 -std=c++17 -O3 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations\
-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported\
-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security\
-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual\
-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel\
-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override\
-Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros\
-Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector\
-fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer\
-pie -fPIE -Werror=vla \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
# --------------------------------------------------------------------------------------------

# --------------------------------------------------------------------------------------------
dump			= Sup/DumpCode/DumpFunc.cpp

table			= Struct/NameTable/NameTableFunc.cpp

parser			= Front/Parser/ParserFunc.cpp Front/Parser/WriteTreeFunc.cpp

support 		= Sup/Support/Support.cpp Sup/Common/NodeFunc.cpp

token 			= Front/Token/TokenFunc.cpp

vector 			= Struct/Vector/Vector.cpp

back			= Back/ParseTree.cpp Back/CompilerFunc.cpp Back/Stack.cpp
# --------------------------------------------------------------------------------------------

# --------------------------------------------------------------------------------------------
common_h		= Sup/Common/

dump_h			= Sup/DumpCode/

table_h			= Struct/NameTable/

parser_h		= Front/Parser/

support_h		= Sup/Support/

token_h 		= Front/Token/

vector_h		= Struct/Vector/

config_h 		= Sup/Configs/

tree_h 			= Back/
# --------------------------------------------------------------------------------------------

# --------------------------------------------------------------------------------------------
all_flags =	-I$(common_h) -I$(table_h) -I$(parser_h) -I$(support_h) \
			-I$(token_h) -I$(vector_h) -I$(dump_h) -I$(config_h) -I$(tree_h)  $(flags)
# --------------------------------------------------------------------------------------------

# --------------------------------------------------------------------------------------------
lang:
	g++ $(all_flags) main.cpp $(vector) $(token) $(support) $(table) $(dump) $(parser) $(back) -o System/lang.exe

run-l:
	./System/lang.exe
# --------------------------------------------------------------------------------------------

# --------------------------------------------------------------------------------------------
test-table:
	g++ $(all_flags) testTable.cpp $(table) $(support) -o test-table.exe

run-t:
	./test-table.exe
