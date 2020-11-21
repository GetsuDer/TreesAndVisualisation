TESTDIR = Testing/
SRCDIR = Source/
OBJDIR = ObjectFiles/
INCDIR = Include/
CC = g++
DEBUG = NO
CFLAGS = -Wall -Wextra -Wformat -std=c++14 -IInclude -ISource 

ifeq ($(DEBUG), YES)
	CFLAGS += -g
endif

.PHONY: all clean

all: tree
	
$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf *.o ObjectFiles
