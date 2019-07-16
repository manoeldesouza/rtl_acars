LIBS= -lrtlsdr -lm

all:
	gcc -Wall -g -std=c11 -o rtl_acars main.c main.h rtl.c channel.c msk.c acars.c -lrtlsdr -lm
