PROG = amidimap
CFLAGS = -g -O2 -Wall -I../alsaprint
CC = g++
LIBS = -lasound



OBJS = \
	amidimap.o

$(PROG): $(OBJS) ../alsaprint/alsaprint.o 
	$(CC) -o $(PROG) $(OBJS) $(LIBS) ../alsaprint/alsaprint.o

amidimap.o: amidimap.c ../alsaprint/alsaprint.h

clean:
	$(RM) $(PROG) $(OBJS)
