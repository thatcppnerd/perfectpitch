main: main.c 
	gcc main.c -o main \
	-lncurses -lSDL2 -lSDL2_mixer \
	-Wall


clean:
	rm main