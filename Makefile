CC=gcc
IDIR=./include
CFLAGS=-I$(IDIR) -Wall -Werror
ODIR=obj

_OBJ = main.o scene.o background.o list.o player.o laser_beam.o util.o enemy.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_DEPS = scene.h player.h list.h background.h util.h laser_beam.h util.h enemy.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

LIBS=-lSDL2 -lSDL2_image

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

game: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o