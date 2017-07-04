OUT=textTc.exe

all: build

build: textTc.o
	gcc -o $(OUT) textTc.c -lm
	rm -rf *.o

textTc.o: textTc.c
	gcc -c textTc.c
	
clean: 
	rm -f *.o $(OUT)

