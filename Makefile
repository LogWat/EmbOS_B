AS     = arm-none-eabi-as
CC     = arm-none-eabi-gcc
LIBGCC = `arm-none-eabi-gcc -print-libgcc-file-name`
CFLAGS = -Wall -O -fno-builtin -fomit-frame-pointer -finhibit-size-directive \
	-fno-ident

all: test.bin

test.bin: ball.o box.o crt.o main.o racket.o game.o block.o draw.o
	arm-none-eabi-ld -o test.out -T gcc.ls \
	  crt.o ball.o box.o main.o racket.o game.o block.o draw.o
	arm-none-eabi-objcopy -O binary test.out test.bin

clean:
	rm -f *.o *.s *.out

ball.o: gba.h box.h ball.h game.h
box.o: gba.h box.h
main.o: gba.h ball.h box.h racket.h game.h block.h
racket.o: gba.h box.h ball.h racket.h game.h
game.o: gba.h game.h
block.o: gba.h ball.h box.h block.h game.h
draw.o: gba.h 8x8.til game.h