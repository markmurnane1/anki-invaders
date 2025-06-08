
#include "hiragana.h"

typedef struct {
    const char *romaji;
    const char *hiragana;
} RomajiPair;

// Romaji to Hiragana conversion table
static const RomajiPair romaji_table[] = {
    // Combined characters first (longer matches)
    {"kya", "きゃ"}, {"kyu", "きゅ"}, {"kyo", "きょ"},
    {"sha", "しゃ"}, {"shu", "しゅ"}, {"sho", "しょ"},
    {"cha", "ちゃ"}, {"chu", "ちゅ"}, {"cho", "ちょ"},
    {"nya", "にゃ"}, {"nyu", "にゅ"}, {"nyo", "にょ"},
    {"hya", "ひゃ"}, {"hyu", "ひゅ"}, {"hyo", "ひょ"},
    {"mya", "みゃ"}, {"myu", "みゅ"}, {"myo", "みょ"},
    {"rya", "りゃ"}, {"ryu", "りゅ"}, {"ryo", "りょ"},
    {"gya", "ぎゃ"}, {"gyu", "ぎゅ"}, {"gyo", "ぎょ"},
    {"ja", "じゃ"}, {"ju", "じゅ"}, {"jo", "じょ"},
    {"bya", "びゃ"}, {"byu", "びゅ"}, {"byo", "びょ"},
    {"pya", "ぴゃ"}, {"pyu", "ぴゅ"}, {"pyo", "ぴょ"},
    
    // Basic characters
    {"ka", "か"}, {"ki", "き"}, {"ku", "く"}, {"ke", "け"}, {"ko", "こ"},
    {"ga", "が"}, {"gi", "ぎ"}, {"gu", "ぐ"}, {"ge", "げ"}, {"go", "ご"},
    {"sa", "さ"}, {"shi", "し"}, {"su", "す"}, {"se", "せ"}, {"so", "そ"},
    {"za", "ざ"}, {"ji", "じ"}, {"zu", "ず"}, {"ze", "ぜ"}, {"zo", "ぞ"},
    {"ta", "た"}, {"chi", "ち"}, {"tsu", "つ"}, {"te", "て"}, {"to", "と"},
    {"da", "だ"}, {"dzi", "ぢ"}, {"dzu", "づ"}, {"de", "で"}, {"do", "ど"},
    {"na", "な"}, {"ni", "に"}, {"nu", "ぬ"}, {"ne", "ね"}, {"no", "の"},
    {"ha", "は"}, {"hi", "ひ"}, {"fu", "ふ"}, {"he", "へ"}, {"ho", "ほ"},
    {"ba", "ば"}, {"bi", "び"}, {"bu", "ぶ"}, {"be", "べ"}, {"bo", "ぼ"},
    {"pa", "ぱ"}, {"pi", "ぴ"}, {"pu", "ぷ"}, {"pe", "ぺ"}, {"po", "ぽ"},
    {"ma", "ま"}, {"mi", "み"}, {"mu", "む"}, {"me", "め"}, {"mo", "も"},
    {"ya", "や"}, {"yu", "ゆ"}, {"yo", "よ"},
    {"ra", "ら"}, {"ri", "り"}, {"ru", "る"}, {"re", "れ"}, {"ro", "ろ"},
    {"wa", "わ"}, {"wo", "を"}, {"n", "ん"},
    
    // Vowels
    {"a", "あ"}, {"i", "い"}, {"u", "う"}, {"e", "え"}, {"o", "お"},
    
    // Small tsu for doubled consonants
    {"kk", "っk"}, {"ss", "っs"}, {"tt", "っt"}, {"pp", "っp"},
    {"gg", "っg"}, {"zz", "っz"}, {"dd", "っd"}, {"bb", "っb"},
    
    {NULL, NULL}
};


// void romaji_to_hiragana(const char *romaji, char *hiragana, size_t size) {
//     if (!romaji || size == 0) return;

//     hiragana[0] = '\0';
//     char temp[INPUT_BUFFER_SIZE];
//     strcpy(temp, romaji);
    
//     char result[INPUT_BUFFER_SIZE] = "";
//     char pending[MAX_ROMAJI_LENGTH] = "";
//     int pending_len = 0;
    
//     for (size_t i = 0; i < strlen(temp); i++) {
//         // Add character to pending
//         pending[pending_len++] = temp[i];
//         pending[pending_len] = '\0';
        
//         int found = 0;
//         int partial_match = 0;
        
//         // Check if pending matches any romaji
//         for (int j = 0; romaji_table[j].romaji != NULL; j++) {
//             if (strcmp(pending, romaji_table[j].romaji) == 0) {
//                 // Exact match found
//                 strcat(result, romaji_table[j].hiragana);
//                 pending[0] = '\0';
//                 pending_len = 0;
//                 found = 1;
//                 break;
//             } else if (strncmp(pending, romaji_table[j].romaji, pending_len) == 0) {
//                 // Partial match - keep building
//                 partial_match = 1;
//             }
//         }
        
//         // If no match and no partial match, output first char and retry rest
//         if (!found && !partial_match && pending_len > 0) {
//             // Output first character as-is
//             char single[2] = {pending[0], '\0'};
//             strcat(result, single);
            
//             // Shift pending buffer
//             for (int k = 0; k < pending_len - 1; k++) {
//                 pending[k] = pending[k + 1];
//             }
//             pending_len--;
//             pending[pending_len] = '\0';
            
//             // Retry remaining
//             i--;
//         }
//     }
    
//     // Append any remaining pending characters
//     strcat(result, pending);
    
//     strncpy(hiragana, result, size - 1);
//     hiragana[size - 1] = '\0';
// }



void romaji_to_hiragana(const char *romaji, char *hiragana, size_t size) {
    hiragana[0] = '\0';
    if (!romaji || size == 0) return;
    
    char result[INPUT_BUFFER_SIZE] = "";
    char pending[MAX_ROMAJI_LENGTH] = "";
    int pending_len = 0;
    
    int i = 0;
    while (i < strlen(romaji)) {
        // Add character to pending
        if (pending_len < MAX_ROMAJI_LENGTH - 1) {
            pending[pending_len++] = romaji[i];
            pending[pending_len] = '\0';
        } else {
            // Pending buffer full - output first char and continue
            char single[2] = {pending[0], '\0'};
            strcat(result, single);
            
            // Shift pending buffer
            for (int k = 0; k < pending_len - 1; k++) {
                pending[k] = pending[k + 1];
            }
            pending_len--;
            pending[pending_len] = '\0';
            continue;
        }
        
        int found = 0;
        int partial_match = 0;
        
        // Check if pending matches any romaji
        for (int j = 0; romaji_table[j].romaji != NULL; j++) {
            if (strcmp(pending, romaji_table[j].romaji) == 0) {
                // Exact match found
                strcat(result, romaji_table[j].hiragana);
                pending[0] = '\0';
                pending_len = 0;
                found = 1;
                break;
            } else if (strncmp(pending, romaji_table[j].romaji, pending_len) == 0 &&
                       strlen(romaji_table[j].romaji) > pending_len) {
                // Partial match - keep building
                partial_match = 1;
            }
        }
        
        // If we found a match or have potential for a match, continue
        if (found || partial_match) {
            i++;
            continue;
        }
        
        // No match and no partial match
        if (pending_len > 0) {
            // Check if we should try without the last character
            if (pending_len > 1) {
                // Remove last character and try again
                pending_len--;
                pending[pending_len] = '\0';
                continue;  // Don't increment i, retry with shorter pending
            } else {
                // Single character with no match - output as-is
                strcat(result, pending);
                pending[0] = '\0';
                pending_len = 0;
                i++;
            }
        }
    }
    
    // Append any remaining pending characters
    strcat(result, pending);
    
    // Copy result to output buffer
    strncpy(hiragana, result, size - 1);
    hiragana[size - 1] = '\0';
}