# EDIT THIS TO MATCH YOUR COMPILER PATH, THIS IS FOR LINUX RIGHT NOW
CC := /usr/bin/gcc

EXEC := ./build/main.out

SRCDIR := ./src

BINDIR := ./bin
LIBDIR := ./lib

CFLAGS := -Wall -g -I$(SRCDIR)/ -I$(LIBDIR)/
LFLAGS := -L$(SRCDIR)/ -L$(BINDIR)/
LDFLAGS := -lraylib -ldl -lGL -lpthread -lm

SRCS := $(shell find $(SRCDIR) -name *.c)
OBJS := $(SRCS:.c=.o)

RM := rm -rf
MKDIR := mkdir -p

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

$(EXEC): $(OBJS)
	$(MKDIR) $(@D)
	$(CC) $^ $(CFLAGS) $(LFLAGS) -o $(EXEC) $(LDFLAGS)
	$(MAKE) clean

build: $(EXEC)

clean:
	$(RM) $(SRCDIR)/*.o