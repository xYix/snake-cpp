snakegame: main.o game.o snake.o render.o leaderboard.o
	g++ -o snakegame main.o game.o render.o leaderboard.o snake.o -lcurses
main.o: main.cpp game.h
	g++ -c main.cpp
render.o: render.cpp
	g++ -c render.cpp
leaderboard.o: leaderboard.cpp
	g++ -c leaderboard.cpp
game.o: game.cpp snake.h
	g++ -c game.cpp
snake.o: snake.cpp
	g++ -c snake.cpp
clean:
	rm *.o 
	rm snakegame
	rm record.dat
