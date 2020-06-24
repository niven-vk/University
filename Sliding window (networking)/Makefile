
Flags = -std=c++14 -Wall -Wextra
CPP = g++

transport : Makefile main.o Funcs.o
	$(CPP) $(Flags) -o transport main.o Funcs.o

main.o : Makefile main.cpp Funcs.o
	$(CPP) -c $(Flags) main.cpp -o  main.o

Funcs.o : Makefile Funcs.cpp Funcs.h
	$(CPP) -c $(Flags) Funcs.cpp  -o Funcs.o 

clean:
	rm -rf *.o

distclean:
	rm -rf *.o transport .*.swp
