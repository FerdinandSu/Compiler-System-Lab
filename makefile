LEX = flex
YYAC = bison
CC = gcc

all: clean parser

parser: main.c syntax.tab.c lex.yy.c
	$(CC) main.c -lfl -ly -g -o parser
syntax.tab.c syntax.tab.h: syntax.y
	$(YYAC) -vd syntax.y

lex.yy.c: lexical.l
	$(LEX) -o $@ $<

scanner.o: scanner.c parser.tab.h

scanner: main.c lex.yy.c
	$(CC) -o $@ main.c lex.yy.c -lfl

.PHONY : clean head all testreq testopt test debug

head: clean syntax.tab.h

test: testreq testopt

testreq: all
	echo "\n\033[34m<1.c>\033[0m\n" && ./parser ./examples/lab1/1.c \
	&& echo "\n\033[34m<2.c>\033[0m\n" && ./parser ./examples/lab1/2.c \
	&& echo "\n\033[34m<3.c>\033[0m\n" && ./parser ./examples/lab1/3.c \
	&& echo "\n\033[34m<4.c>\033[0m\n" && ./parser ./examples/lab1/4.c \
	&& echo "\n\033[34m<5.c>\033[0m\n" && ./parser ./examples/lab1/5.c

testopt: all
	echo "\n\033[34m<6.c>\033[0m\n" && ./parser ./examples/lab1/6.c \
	&& echo "\n\033[34m<7.c>\033[0m\n" && ./parser ./examples/lab1/7.c \
	&& echo "\n\033[34m<8.c>\033[0m\n" && ./parser ./examples/lab1/8.c \
	&& echo "\n\033[34m<9.c>\033[0m\n" && ./parser ./examples/lab1/9.c \
	&& echo "\n\033[34m<10.c>\033[0m\n" && ./parser ./examples/lab1/10.c
clean:
	-@ rm parser syntax.tab.c syntax.tab.h lex.yy.c syntax.output *.o

debug: clean test