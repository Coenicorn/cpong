# edit this to point to your preferred compiler
#
# makefile template expects filesystem like so:
# 
# parent dir
# 	- src
# 		- main.c
# 	- bin
# 		- libmylibrary.a
# 	- lib
# 		- mylibrary.h
# 	- obj
# 	- build

CC := /usr/bin/clang

EXEC := ./build/main.out

SRCDIR := ./src
OBJDIR := ./obj
INCLUDEDIR := ./include
LIBDIR := ./lib

CFLAGS := -Wall -Wextra -g -I$(SRCDIR)/ -I$(INCLUDEDIR)/
LFLAGS := -L$(SRCDIR)/ -L$(LIBDIR)/
LDFLAGS := -lraylib -lGL -ldl -lpthread -lm

OBJS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(OBJS:.c=.o)
OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(OBJS))

MKDIR := mkdir -p
RM := rm -rf

$(EXEC): $(OBJS)
	$(MKDIR) $(@D)
	$(CC) $^ $(CFLAGS) $(LFLAGS) -o $(EXEC) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $< $(CFLAGS) -o $@

run: $(EXEC)
	@clear
	$(EXEC)

clean:
	$(RM) $(EXEC)
	$(RM) $(OBJS)
