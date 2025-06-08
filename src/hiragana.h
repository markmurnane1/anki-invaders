#ifndef INPUT_H
#define INPUT_H

#include <string.h>
#include <stdlib.h>

#define INPUT_BUFFER_SIZE 256
#define MAX_ROMAJI_LENGTH 10

void romaji_to_hiragana(const char *romaji, char *hiragana, size_t size);

#endif
