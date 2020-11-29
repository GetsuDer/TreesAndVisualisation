TESTDIR = Testing/
SRCDIR = Source/
OBJDIR = ObjectFiles/
INCDIR = Include/
CC = g++
DEBUG = NO
CFLAGS = -Wall -Wextra -Wformat -std=c++14 -IInclude 

ifeq ($(DEBUG), YES)
	CFLAGS += -g
endif

.PHONY: all clean tree

all: tree
	
test: tree
	cd Testing; ./run_tests; cd ..

tree: $(OBJDIR)main.o $(OBJDIR)tree.o $(OBJDIR)in_and_out.o
	$(CC) -o tree $(OBJDIR)tree.o $(OBJDIR)main.o $(OBJDIR)in_and_out.o $(CFLAGS)

$(OBJDIR)tree.o: $(SRCDIR)tree.cpp $(OBJDIR) $(INCDIR)tree.h
	$(CC) -c -o $(OBJDIR)tree.o $(SRCDIR)tree.cpp $(CFLAGS)

$(OBJDIR)main.o: $(SRCDIR)main.cpp $(OBJDIR) $(INCDIR)tree.h
	$(CC) -c -o $(OBJDIR)main.o $(SRCDIR)main.cpp $(CFLAGS)

$(OBJDIR)in_and_out.o: $(SRCDIR)in_and_out.cpp $(OBDJIR) $(INCDIR)in_and_out.h
	$(CC) -c -o $(OBJDIR)in_and_out.o $(SRCDIR)in_and_out.cpp $(CFLAFS)
$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf *.o ObjectFiles tree Testing/*.log Testing/*.dot Testing/*.tex
