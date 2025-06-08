#ifndef CARD_H
#define CARD_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


// Structure to hold card data
typedef struct {
    char *word;
    char *word_reading;
    char *word_meaning;
} CardData;

int parse_card_fields(const char *fields_str, CardData *card); 
void free_card_data(CardData *card);

#endif
