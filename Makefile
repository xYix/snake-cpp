snakegame: main.o game.o snake.o render.o leaderboard.o boss.o
	g++ -o snakegame main.o game.o snake.o render.o leaderboard.o boss.o -lcurses
main.o: main.cpp game.h
	g++ -c main.cpp
render.o: render.cpp
	g++ -c render.cpp
leaderboard.o: leaderboard.cpp
	g++ -c leaderboard.cpp
game.o: game.cpp snake.o
	g++ -c game.cpp
snake.o: snake.cpp
	g++ -c snake.cpp
boss.o: boss.cpp snake.o
	g++ -c boss.cpp
clean:
	rm *.o 
	rm snakegame
	rm record.dat
