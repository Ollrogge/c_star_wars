CC=gcc
IDIR=./include
CFLAGS=-I$(IDIR) -Wall -Werror
ODIR=obj

_OBJ = main.o scene.o background.o list.o player.o laser_beam.o util.o enemy.o text.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_DEPS = scene.h player.h list.h background.h util.h laser_beam.h util.h enemy.h text.h object.h text.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

game: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o