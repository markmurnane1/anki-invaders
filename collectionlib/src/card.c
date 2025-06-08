#include "../include/card.h"

#define FIELD_SEPARATOR "\x1f"  // Anki uses this separator between fields

// Function to extract first meaning from HTML
char* extract_first_meaning_from_html(const char *html_str) {
    if (!html_str) return NULL;
    
    // Look for yomitan-glossary class
    const char *glossary_start = strstr(html_str, "yomitan-glossary");
    if (!glossary_start) {
        // No HTML structure, return as is
        return strdup(html_str);
    }
    
    // Find the first inner <div> after yomitan-glossary
    const char *first_div = strstr(glossary_start, "<div>");
    if (!first_div) return NULL;
    
    // Move past the opening <div>
    first_div += 5; // length of "<div>"
    
    // Find the closing </div>
    const char *div_end = strstr(first_div, "</div>");
    if (!div_end) return NULL;
    
    // Calculate length of content
    size_t content_len = div_end - first_div;
    
    // Allocate and copy the content
    char *result = (char *)malloc(content_len + 1);
    if (result) {
        strncpy(result, first_div, content_len);
        result[content_len] = '\0';
    }
    
    return result;
}

// Function to parse fields from the note
int parse_card_fields(const char *fields_str, CardData *card) {
    if (!fields_str || !card) return -1;
    
    // Make a copy of the string to tokenize
    char *fields_copy = strdup(fields_str);
    if (!fields_copy) return -1;
    
    // Initialize card fields
    card->word = NULL;
    card->word_reading = NULL;
    card->word_meaning = NULL;
    
    // Parse fields using the separator
    char *saveptr;
    char *token;
    int field_index = 0;
    
    token = strtok_r(fields_copy, FIELD_SEPARATOR, &saveptr);
    while (token != NULL && field_index < 13) {
        // Trim whitespace
        while (*token && isspace(*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace(*end)) *end-- = '\0';
        
        switch (field_index) {
            case 0:  // Word
                card->word = strdup(token);
                break;
            case 1:  // Word Reading
                card->word_reading = strdup(token);
                break;
            case 2:  // Word Meaning
                card->word_meaning = extract_first_meaning_from_html(token);
                break;
            // Skip other fields (3-12)
        }
        
        field_index++;
        token = strtok_r(NULL, FIELD_SEPARATOR, &saveptr);
    }
    
    free(fields_copy);
    
    // Check if we got all required fields
    if (!card->word || !card->word_reading || !card->word_meaning) {
        free_card_data(card);
        return -1;
    }
    
    return 0;
}

// Function to free card data
void free_card_data(CardData *card) {
    if (card->word) {
        free(card->word);
        card->word = NULL;
    }
    if (card->word_reading) {
        free(card->word_reading);
        card->word_reading = NULL;
    }
    if (card->word_meaning) {
        free(card->word_meaning);
        card->word_meaning = NULL;
    }
}
