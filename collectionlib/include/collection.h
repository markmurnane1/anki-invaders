#ifndef COLLECTION_H
#define COLLECTION_H

#include <sqlite3.h>
#include <stddef.h>
#include <stdio.h>

#include "../include/card.h"

#define MAX_CARDS 50

// Structure to hold all cards
typedef struct {
    sqlite3 *db;
    CardData cards[MAX_CARDS];
    int count;
} CardCollection;

CardCollection* setup_collection(const char *db_path, const char *deck_name);
void delete_collection(CardCollection *collection);

#endif