test: main.o playgame.o bot.o
	g++ -g -o test main.o playgame.o bot.o -lpthread
 
main.o: main.cpp playgame.h bot.h
	g++ -g -c main.cpp
 
playgame.o: playgame.c playgame.h
	gcc -g -c playgame.c
 
bot.o: bot.c bot.h playgame.h
	gcc -g -c bot.c
 
clean:
	rm -f *.o test
 