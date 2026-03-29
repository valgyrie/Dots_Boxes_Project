test: main.o playgame.o
	gcc -g -o test main.o playgame.o

main.o: main.c playgame.h
	gcc -g -c main.c

playgame.o: playgame.c playgame.h
	gcc -g -c playgame.c

