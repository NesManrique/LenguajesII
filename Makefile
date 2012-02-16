LEX    = flex
BISON  = bison
BFLAGS = --output=parser.cpp

comp:	parser.cpp lexer.cpp main.cpp ast.h symtable.h
		g++ $^ -o $@ -lm -ly -Wno-deprecated

deb: 
		bison -o parser.cpp -vtdy parser.y
		flex -o lexer.cpp  lexer.l parser.hpp
		g++ parser.cpp lexer.cpp main.cpp ast.h symtable.h -o comp -lm -ly -Wno-deprecated -ggdb -DDEBUG

parser.cpp:	parser.y
		bison -o $@ -dy $^

parser.hpp : parser.cpp

lexer.cpp:	lexer.l parser.hpp
		flex -o $@  $^ 

clean:
		rm comp parser.hpp lexer.cpp parser.cpp
