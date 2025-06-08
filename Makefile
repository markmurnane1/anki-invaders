# game/Makefile

CC = gcc
CFLAGS = -Wall -Wextra -I../collectionlib/include
LDFLAGS = -Lcollectionlib/lib -lcollection -lsqlite3
OBJDIR = build
BINDIR = bin

SRC = src/main.c
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
