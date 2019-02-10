.PHONY: all clean 
all: libstatic.a libdyn1.so libdyn2.so 
	gcc main.c -o result -L. -lstatic -ldyn1 -ldyn2 -Wl,-rpath,. -ldl
libdyn1.so: sub.o
	gcc -shared -o libdyn1.so sub.o
libdyn2.so: mult.o 
	gcc -shared -o libdyn2.so mult.o
libstatic.a: sum.o
	ar rcs libstatic.a sum.o
sum.o: sum.c
	gcc -c sum.c 
sub.o: sub.c
	gcc -fPIC -c sub.c 
mult.o: mult.c
	gcc -fPIC -c mult.c
clean: 
	rm -f *.o *.a 
