IDIR = include
ODIR = obj
SDIR = src
CC = g++
CFLAGS = -g -Wall -I $(IDIR)
SOURCES = $(SDIR)/*.cc
LIBS = `pkg-config --cflags --libs libserial`

_DEPS = serial.h util.h pn532.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o serial.o util.o pn532.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: $(OBJ)
	$(CC) $(CFLAGS) $(SOURCES) -o ticket-hack $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o

$(OBJ): $(ODIR)/%.o: $(SDIR)/%.cc $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

