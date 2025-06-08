# collectionlib/Makefile

CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -fPIC -Iinclude
LDFLAGS = -lsqlite3
OBJDIR = build
LIBDIR = lib

SRC = src/card.c src/collection.c
OBJ = $(SRC:%.c=$(OBJDIR)/%.o)

STATIC_LIB = $(LIBDIR)/libcollection.a
SHARED_LIB = $(LIBDIR)/libcollection.so

all: $(STATIC_LIB)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB): $(OBJ)
	@mkdir -p $(LIBDIR)
	$(AR) rcs $@ $^

$(SHARED_LIB): $(OBJ)
	@mkdir -p $(LIBDIR)
	$(CC) -shared -o $@ $^

clean:
	rm -rf $(OBJDIR) $(LIBDIR)

.PHONY: all clean
