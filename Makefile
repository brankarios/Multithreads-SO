# pf1 1: Minishell #
# César Carios & Jhonatan Homsany #

all: pf1

pf1: pf1.o
	gcc -o pf1 pf1.o

pf1.o: pf1.c pf1.h 
	gcc -c pf1.c

clean:
	rm -rf pf1 pf1.o
