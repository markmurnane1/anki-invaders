
#include "../include/collection.h"
#include <stdio.h>

// Function to free all cards
void free_card_collection(CardCollection *collection) {
    for (int i = 0; i < collection->count; i++) {
        free_card_data(&collection->cards[i]);
    }
    collection->count = 0;
}

// Function to extract cards from a deck
int extract_cards_from_deck(size_t deck_id, CardCollection *collection) {
    sqlite3 *db = collection->db;
    sqlite3_stmt *stmt;
    const char *sql = 
        "SELECT n.id, n.flds, c.id "
        "FROM cards c "
        "JOIN notes n ON c.nid = n.id "
        "WHERE c.did = ? "
        "LIMIT ?;";
    
    collection->count = 0;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare cards query: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int64(stmt, 1, deck_id);
    sqlite3_bind_int(stmt, 2, MAX_CARDS);
    
    printf("\n=== EXTRACTING CARDS FROM DECK %zu ===\n", deck_id);
    
    while (sqlite3_step(stmt) == SQLITE_ROW && collection->count < MAX_CARDS) {
        long long note_id = sqlite3_column_int64(stmt, 0);
        const char *fields = (const char*)sqlite3_column_text(stmt, 1);
        long long card_id = sqlite3_column_int64(stmt, 2);
        
        if (!fields) continue;
        
        CardData *card = &collection->cards[collection->count];
        
        if (parse_card_fields(fields, card) == 0) {
            printf("\n--- Card %d (Card ID: %lld, Note ID: %lld) ---\n", 
                   collection->count + 1, card_id, note_id);
            printf("Word: %s\n", card->word);
            printf("Word Reading: %s\n", card->word_reading);
            printf("Word Meaning: %s\n", card->word_meaning);
            
            collection->count++;
        }
    }
    
    sqlite3_finalize(stmt);
    
    printf("\n=== TOTAL CARDS EXTRACTED: %d ===\n", collection->count);
    
    return collection->count;
}

size_t find_deck_by_name(sqlite3 *db, const char *target_deck_name) {
    sqlite3_stmt *stmt;
    size_t ret = 0;
    
    // Now query the decks table directly
    printf("\n=== QUERYING DECKS TABLE ===\n");
    const char *decks_sql = "SELECT id, name, mtime_secs, usn FROM decks;";
    
    if (sqlite3_prepare_v2(db, decks_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare decks query: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        long long deck_id = sqlite3_column_int64(stmt, 0);
        const char *deck_name = (const char*)sqlite3_column_text(stmt, 1);
        long long mtime = sqlite3_column_int64(stmt, 2);
        int usn = sqlite3_column_int(stmt, 3);
        
        if (!deck_name) continue;
        
        // Check if this is the target deck
        int is_target = (target_deck_name && strcmp(deck_name, target_deck_name) == 0);
        
        // Print deck information
        printf("Deck ID: %lld\n", deck_id);
        printf("Deck Name: %s\n", deck_name);
        printf("Modified Time: %lld\n", mtime);
        printf("USN: %d\n", usn);
        printf("---\n");
        
        if (is_target) {
            ret = deck_id;
        }
    }
    
    sqlite3_finalize(stmt);
    return ret;
}

CardCollection* setup_collection(const char *db_path, const char *deck_name) {

    CardCollection *collection = malloc(sizeof(CardCollection));
    if (!collection) {
        fprintf(stderr, "Failed to allocate collection\n");
        return NULL;
    }

    // Open database
    if (sqlite3_open(db_path, &collection->db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(collection->db));
        return NULL;
    }
    
    printf("Opened Anki collection: %s\n\n", db_path);
    size_t deck_id = find_deck_by_name(collection->db, deck_name);
    
    if (deck_id == 0) {
        printf("Deck not found.\n");
        sqlite3_close(collection->db);
        return NULL;
    }
    
    printf("Target deck found at ID: %zu\n", deck_id);
    
    // Extract cards from the deck
    int cards_extracted = extract_cards_from_deck(deck_id, collection);
    
    if (cards_extracted < 0) {
        printf("Error extracting cards.\n");
        free_card_collection(collection);
        sqlite3_close(collection->db);
        return NULL;
    }
    
    // The cards are now stored in memory in the collection structure
    printf("\n=== CARDS SUCCESSFULLY LOADED INTO MEMORY ===\n");
    printf("You can now access the cards through the collection structure.\n");
    printf("Example: collection.cards[0].word = \"%s\"\n", 
           collection->count > 0 ? collection->cards[0].word : "N/A");

    return collection;
}

void delete_collection(CardCollection *collection) {
    // Clean up
    free_card_collection(collection);
    sqlite3_close(collection->db);    
}
