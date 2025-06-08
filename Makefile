# game/Makefile

CC = gcc
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs) -lSDL2_ttf

CFLAGS = -Wall -Wextra -I../collectionlib/include $(SDL_CFLAGS)
LDFLAGS = -Lcollectionlib/lib -lcollection -lsqlite3 $(SDL_LDFLAGS)

OBJDIR = build
BINDIR = bin

SRC = src/main.c src/hiragana.c
OBJ = $(SRC:%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/game

all: $(TARGET)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
