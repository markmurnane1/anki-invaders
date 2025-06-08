#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include <locale.h>

#include "../collectionlib/include/collection.h"
#include "hiragana.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_ENEMIES 10
#define ENEMY_SPEED 30.0f
#define SPAWN_DELAY 6000
#define SHOW_MEANING_DURATION 2000

typedef struct {
    float x, y;
    int card_index;
    int alive;
    int showing_meaning;
    Uint32 death_time;
} Enemy;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font_large;
    TTF_Font *font_medium;
    TTF_Font *font_small;
    
    Enemy enemies[MAX_ENEMIES];
    char input_buffer[INPUT_BUFFER_SIZE];
    char romaji_buffer[INPUT_BUFFER_SIZE];
    char display_buffer[INPUT_BUFFER_SIZE];
    int input_length;
    
    int game_over;
    int score;
    Uint32 last_spawn_time;
    
    CardCollection *collection;
} GameState;


void init_enemy(Enemy *enemy, int card_index, float x) {
    enemy->x = x;
    enemy->y = -50;
    enemy->card_index = card_index;
    enemy->alive = 1;
    enemy->showing_meaning = 0;
    enemy->death_time = 0;
}

void spawn_enemy(GameState *game) {
    if (game->collection->count == 0) return;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].alive && !game->enemies[i].showing_meaning) {
            int card_index = rand() % game->collection->count;
            float x = 50 + (rand() % (WINDOW_WIDTH - 100));
            init_enemy(&game->enemies[i], card_index, x);
            break;
        }
    }
}

void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, 
                 int x, int y, SDL_Color color) {
    if (!text || strlen(text) == 0) return;
    
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) return;
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_Rect dest = {x - surface->w / 2, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void update_enemies(GameState *game, float delta_time) {
    Uint32 current_time = SDL_GetTicks();
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *enemy = &game->enemies[i];
        
        if (enemy->showing_meaning) {
            if (current_time - enemy->death_time > SHOW_MEANING_DURATION) {
                enemy->showing_meaning = 0;
            }
        } else if (enemy->alive) {
            enemy->y += ENEMY_SPEED * delta_time;
            
            if (enemy->y > WINDOW_HEIGHT - 50) {
                game->game_over = 1;
            }
        }
    }
}

void check_input(GameState *game) {
    if (strlen(game->input_buffer) == 0) return;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *enemy = &game->enemies[i];
        if (!enemy->alive || enemy->showing_meaning) continue;
        
        CardData *card = &game->collection->cards[enemy->card_index];
        
        if (strcmp(game->input_buffer, card->word_reading) == 0) {
            enemy->alive = 0;
            enemy->showing_meaning = 1;
            enemy->death_time = SDL_GetTicks();
            game->score += 100;
            
            // Clear input buffers
            game->input_buffer[0] = '\0';
            game->romaji_buffer[0] = '\0';
            game->display_buffer[0] = '\0';
            game->input_length = 0;
            break;
        }
    }
}

void render_game(GameState *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);
    
    // Render enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *enemy = &game->enemies[i];
        if (!enemy->alive && !enemy->showing_meaning) continue;
        
        CardData *card = &game->collection->cards[enemy->card_index];
        
        if (enemy->showing_meaning) {
            // Show meaning in green
            SDL_Color green = {0, 255, 0, 255};
            render_text(game->renderer, game->font_medium, 
                       card->word_meaning, (int)enemy->x, (int)enemy->y, green);
        } else {
            // Show kanji in white
            SDL_Color white = {255, 255, 255, 255};
            render_text(game->renderer, game->font_large, 
                       card->word, (int)enemy->x, (int)enemy->y, white);
        }
    }
    
    // Render converted hiragana
    SDL_Color yellow = {255, 255, 0, 255};
    render_text(game->renderer, game->font_medium, game->display_buffer, 
               WINDOW_WIDTH / 2, WINDOW_HEIGHT - 120, yellow);
    
    // Render romaji input
    SDL_Color cyan = {0, 255, 255, 255};
    render_text(game->renderer, game->font_small, game->romaji_buffer, 
               WINDOW_WIDTH / 2, WINDOW_HEIGHT - 80, cyan);
    
    // Render score
    char score_text[64];
    snprintf(score_text, sizeof(score_text), "Score: %d", game->score);
    SDL_Color white = {255, 255, 255, 255};
    render_text(game->renderer, game->font_small, score_text, 100, 30, white);
    
    // Render game over
    if (game->game_over) {
        SDL_Color red = {255, 0, 0, 255};
        render_text(game->renderer, game->font_large, "GAME OVER", 
                   WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, red);
    }
    
    SDL_RenderPresent(game->renderer);
}

int main(int argc, char *argv[]) {
    // Set locale for UTF-8 support
    setlocale(LC_ALL, "");
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
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    
    if (TTF_Init() < 0) {
        printf("TTF initialization failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    
    GameState game = {0};
    
    // Create window and renderer
    game.window = SDL_CreateWindow("Japanese Typing Game",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   WINDOW_WIDTH, WINDOW_HEIGHT,
                                   SDL_WINDOW_SHOWN);
    if (!game.window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    game.renderer = SDL_CreateRenderer(game.window, -1, 
                                      SDL_RENDERER_ACCELERATED | 
                                      SDL_RENDERER_PRESENTVSYNC);
    if (!game.renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(game.window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Load fonts (adjust paths as needed)
    game.font_large = TTF_OpenFont("assets/fonts/NotoSansJP-Regular.ttf", 48);
    game.font_medium = TTF_OpenFont("assets/fonts/NotoSansJP-Regular.ttf", 32);
    game.font_small = TTF_OpenFont("assets/fonts/NotoSansJP-Regular.ttf", 24);
    
    if (!game.font_large || !game.font_medium || !game.font_small) {
        printf("Font loading failed: %s\n", TTF_GetError());
        SDL_DestroyRenderer(game.renderer);
        SDL_DestroyWindow(game.window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Initialize game state
    srand(time(NULL));
    game.input_buffer[0] = '\0';
    game.romaji_buffer[0] = '\0';
    game.display_buffer[0] = '\0';
    game.input_length = 0;
    game.game_over = 0;
    game.score = 0;
    game.last_spawn_time = 0;
    game.collection = collection;
    
    // Initialize enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game.enemies[i].alive = 0;
        game.enemies[i].showing_meaning = 0;
    }
    
    // Game loop
    SDL_Event event;
    int running = 1;
    Uint32 last_time = SDL_GetTicks();
    
    while (running) {
        Uint32 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN && !game.game_over) {
                if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(game.romaji_buffer) > 0) {
                    // Remove last character from romaji buffer
                    game.romaji_buffer[strlen(game.romaji_buffer) - 1] = '\0';
                    
                    // Reconvert entire buffer
                    romaji_to_hiragana(game.romaji_buffer, game.input_buffer, INPUT_BUFFER_SIZE);
                    strcpy(game.display_buffer, game.input_buffer);
                } else if (event.key.keysym.sym == SDLK_RETURN) {
                    check_input(&game);
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                } else {
                    // Handle character input
                    char ch = 0;
                    SDL_Keycode key = event.key.keysym.sym;
                    
                    // Convert keycode to character
                    if (key >= SDLK_a && key <= SDLK_z) {
                        ch = 'a' + (key - SDLK_a);
                    }
                    
                    if (ch && strlen(game.romaji_buffer) < INPUT_BUFFER_SIZE - 2) {
                        // Add character to romaji buffer
                        int len = strlen(game.romaji_buffer);
                        game.romaji_buffer[len] = ch;
                        game.romaji_buffer[len + 1] = '\0';
                        
                        // Convert to hiragana
                        romaji_to_hiragana(game.romaji_buffer, game.input_buffer, INPUT_BUFFER_SIZE);
                        strcpy(game.display_buffer, game.input_buffer);
                    }
                }
            }
        }
        
        if (!game.game_over) {
            // Spawn enemies
            if (current_time - game.last_spawn_time > SPAWN_DELAY) {
                spawn_enemy(&game);
                game.last_spawn_time = current_time;
            }
            
            // Update game state
            update_enemies(&game, delta_time);
        }
        
        // Render
        render_game(&game);
    }
    
    // Cleanup
    TTF_CloseFont(game.font_large);
    TTF_CloseFont(game.font_medium);
    TTF_CloseFont(game.font_small);
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}