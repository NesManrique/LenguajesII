LEX    = flex
BISON  = bison
BFLAGS = --output=parser.cpp

comp:	parser.cpp lexer.cpp main.cpp
		g++ $^ node.h -o $@ -lm -ly

parser.cpp:	parser.y
		bison -o $@ -dy $^

parser.hpp : parser.cpp

lexer.cpp:	lexer.l parser.hpp
		flex -o $@  $^ 

clean:
		rm comp parser.hpp lexer.cpp parser.cpp
