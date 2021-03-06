# change application name here (executable output name)
TARGET=template_app
 
# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall
 
PTHREAD=-pthread
 
CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe
 
GTKLIB=`pkg-config --cflags --libs gtk+-3.0`
 
# linker
LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic
 
OBJS=main.o
 
all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)
	gcc server/p_server.c -pthread -o ser

main.o: client/p_client.c
	$(CC) -c $(CCFLAGS) client/p_client.c $(GTKLIB) -o main.o
    
clean:
	rm -f *.o $(TARGET)
	rm -f ser
