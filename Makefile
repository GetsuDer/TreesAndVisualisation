TESTDIR = Testing/
SRCDIR = Source/
OBJDIR = ObjectFiles/
INCDIR = Include/
CC = g++
DEBUG = YES
CFLAGS = -Wall -Wextra -Wformat -std=c++14 -IInclude 

ifeq ($(DEBUG), YES)
	CFLAGS += -g
endif

.PHONY: all clean tree rec_desc

all: tree rec_desc

rec_desc: $(OBJDIR)rec_desc.o $(OBJDIR)main_rec.o $(OBJDIR)visualize.o $(OBJDIR)tree.o $(OBJDIR)in_and_out.o
	$(CC) -o rec_desc $(OBJDIR)rec_desc.o $(OBJDIR)main_rec.o $(OBJDIR)visualize.o $(OBJDIR)tree.o $(OBJDIR)in_and_out.o $(CFLAGS)
	
test: tree
	cd Testing; ./run_tests; cd ..

tree: $(OBJDIR)main.o $(OBJDIR)tree.o $(OBJDIR)in_and_out.o $(OBJDIR)visualize.o
	$(CC) -o tree $(OBJDIR)tree.o $(OBJDIR)main.o $(OBJDIR)in_and_out.o $(OBJDIR)visualize.o $(CFLAGS)

$(OBJDIR)tree.o: $(SRCDIR)tree.cpp $(OBJDIR) $(INCDIR)tree.h
	$(CC) -c -o $(OBJDIR)tree.o $(SRCDIR)tree.cpp $(CFLAGS)

$(OBJDIR)main.o: $(SRCDIR)main.cpp $(OBJDIR) $(INCDIR)tree.h
	$(CC) -c -o $(OBJDIR)main.o $(SRCDIR)main.cpp $(CFLAGS)

$(OBJDIR)in_and_out.o: $(SRCDIR)in_and_out.cpp $(OBDJIR) $(INCDIR)in_and_out.h
	$(CC) -c -o $(OBJDIR)in_and_out.o $(SRCDIR)in_and_out.cpp $(CFLAFS)

$(OBJDIR)rec_desc.o: $(SRCDIR)rec_desc.cpp $(OBJDIR) $(INCDIR)tree.h
	$(CC) -c -o $(OBJDIR)rec_desc.o $(SRCDIR)rec_desc.cpp $(CFLAGS)

$(OBJDIR)main_rec.o: $(SRCDIR)main_rec.cpp $(OBJDIR)
	$(CC) -c -o $(OBJDIR)main_rec.o $(SRCDIR)main_rec.cpp $(CFLAGS)

$(OBJDIR)visualize.o: $(SRCDIR)visualize.cpp $(OBJDIR)
	$(CC) -c -o $(OBJDIR)visualize.o $(SRCDIR)visualize.cpp $(CFLAGS)

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf *.o ObjectFiles tree Testing/*.log Testing/*.dot Testing/*.tex rec_desc
