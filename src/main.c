#include <stdio.h>

#include "../collectionlib/include/collection.h"

int main(int argc, char *argv[]) {
    const char *db_path;
    const char *search_term;
    CardCollection *collection;
    
    // Parse command line arguments
    if (argc < 3) {
        printf("Usage: %s <path_to_collection.anki2> <deck_name>\n", argv[0]);
        return 1;
    }
    
    db_path = argv[1];
    search_term = argv[2];
    
    collection = setup_collection(db_path, search_term);

    // RUN GAME
    
    delete_collection(collection);
       
    return 0;
}