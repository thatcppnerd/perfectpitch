main: main.c
	gcc main.c -o main \
	-lncurses \
	-Wall

clean:
	rm main