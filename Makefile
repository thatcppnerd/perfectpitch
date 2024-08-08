main: *.c 
	gcc *.c -o main \
	-lncurses -lSDL2 -lSDL2_mixer -lm \
	-Wall


clean:
	rm main