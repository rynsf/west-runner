#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define SIZEX 1270
#define SIZEY 720
#define TILE_SIZE 128
#define RUNNER_FRAMERATE 0.15
#define NO_RUNNER_FRAMES 2 //TODO incease the number when the runner sprite take more tiles
#define NO_CACTUS 2
#define INC_SCORE 1

int EndGame();
int RenderText(char* str);

enum {
    TITLE,
    RUNNING,
    ENDGAME
} game_state;

typedef struct {
    SDL_Texture* tex;
    SDL_Rect rect;
    SDL_Rect collison_rect;
} tile;  //Really? is that how you wanna do it, having explicit variable names for all the textures. A better option would be to have an array

SDL_Window* win;
SDL_Renderer* rend;
TTF_Font* font;
SDL_Color black_color = {0, 0, 0, 0};
SDL_Texture* land_tex;

struct {
    SDL_Texture* tex[NO_RUNNER_FRAMES];
    SDL_Rect rect;
    SDL_Rect collison_rect;
} boy;

double current_frame = 0;
int land_len = (SIZEX/128)+2;
tile land[(SIZEX/128)+2];
tile cactus[2]; //TODO Write a better struct for cactus
const double gravity = 0.6;
const int ground = SIZEY-40;
int velx = 8;
double vely = 0;
int flag = 1;
int score = 0;
int islost = 0;

int Init() {
    // TODO Error handling
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    win = SDL_CreateWindow("Runner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SIZEX, SIZEY, 0);
    rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("./assets/PublicPixel.ttf", 32);

    boy.tex[0] = IMG_LoadTexture(rend, "./assets/boy-1.png");
    boy.tex[1] = IMG_LoadTexture(rend, "./assets/boy-2.png");

    land_tex = IMG_LoadTexture(rend, "./assets/land.png");


    //TODO better way to handle cactus...
    cactus[0].tex = IMG_LoadTexture(rend, "./assets/cactus-1.png");

    cactus[1].tex = IMG_LoadTexture(rend, "./assets/cactus-2.png");

    return 0;
}

int InitGame() {
    boy.rect.x = 100; // TODO better way to make rects may be function
    boy.rect.y = ground - TILE_SIZE;
    boy.rect.w = TILE_SIZE;
    boy.rect.h = TILE_SIZE;
    boy.collison_rect.x = boy.rect.x + (7*4); //TODO get rid of magic numbers
    boy.collison_rect.y = boy.rect.y;
    boy.collison_rect.w = 17*4;
    boy.collison_rect.h = 32*4;

    for(int i = 0, x = 0; i < land_len; i++, x += TILE_SIZE) {
        land[i].tex = land_tex;
        land[i].rect.x = x; // TODO better way to make rects may be function
        land[i].rect.y = SIZEY - TILE_SIZE;
        land[i].rect.w = TILE_SIZE;
        land[i].rect.h = TILE_SIZE;
    }

    cactus[0].rect.x = 500; //TODO Remove magic number may be make it random
    cactus[0].rect.y = ground - TILE_SIZE;
    cactus[0].rect.w = TILE_SIZE;
    cactus[0].rect.h = TILE_SIZE;
    cactus[0].collison_rect.x = cactus[0].rect.x + (2*4);
    cactus[0].collison_rect.y = cactus[0].rect.y + (9*4);
    cactus[0].collison_rect.w = 30*4;
    cactus[0].collison_rect.h = 24 * 4;

    cactus[1].rect.x = 1100; //TODO Remove magic number may be make it random
    cactus[1].rect.y = ground - TILE_SIZE;
    cactus[1].rect.w = TILE_SIZE;
    cactus[1].rect.h = TILE_SIZE;
    cactus[1].collison_rect.x = cactus[1].rect.x + (6*4);
    cactus[1].collison_rect.y = cactus[1].rect.y + (3*4);
    cactus[1].collison_rect.w = 20 * 4;
    cactus[1].collison_rect.h = 29 * 4;

    return 0;
}

int check_collison(SDL_Rect rect1, SDL_Rect rect2) {
    return (rect1.x < rect2.x + rect2.w &&
    rect1.x + rect1.w > rect2.x &&
    rect1.y < rect2.y + rect2.h &&
    rect1.y + rect1.h > rect2.y);
}

int Update() {
    for(int i = 0; i < land_len; i++) {
        if(land[i].rect.x <= -TILE_SIZE) {
            land[i].rect.x = (land_len-1) * TILE_SIZE;
            score += INC_SCORE;
        }
    }
    for(int i = 0; i < land_len; i++) {
        land[i].rect.x -= velx;
    }
    for(int i = 0; i < NO_CACTUS; i++) {
        if(cactus[i].rect.x <= -TILE_SIZE) {
            cactus[i].rect.x = cactus[i].rect.x + (land_len * TILE_SIZE);
            cactus[i].collison_rect.x = cactus[i].collison_rect.x + (land_len * TILE_SIZE);
        }
    }
    for(int i = 0; i < NO_CACTUS; i++) {
        cactus[i].rect.x -= velx;
        cactus[i].collison_rect.x -= velx;
    }
    if(boy.rect.y == ground - TILE_SIZE) {
        current_frame += RUNNER_FRAMERATE;
    }
    if(current_frame >= NO_RUNNER_FRAMES) {
        current_frame = 0;
    }
    boy.rect.y -= vely;
    boy.collison_rect.y = boy.rect.y;
    vely -= gravity;
    if(boy.rect.y > ground - TILE_SIZE) {
        vely = 0;
        boy.rect.y = ground - TILE_SIZE;
    }
    if(score > 50 && flag) {
        velx += 2;
        flag = 0;
    }
    return 0;
}

// int RenderText(char* str, SDL_Color color, SDL_Rect dest_rect) {
//
//     return 0; // TODO C is really limiting when it comes to what can be done, instead of just passing values like number if I had
//               // the ability to pass functions that is code as the argument of a function it would be awesome, but hell yeah its C.
// }

int RenderScore() {
    char score_str[20]; //TODO just being lazy correct it...
    sprintf(score_str, "%d", score);
    SDL_Surface* sur = TTF_RenderText_Solid(font, score_str, black_color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(rend, sur);
    int w, h;
    SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);
    SDL_Rect dest_rect = {.x = SIZEX - w - 16, .y = 16, .w = w, .h = h};
    SDL_RenderCopy(rend, text_texture, NULL, &dest_rect);
    SDL_FreeSurface(sur);
    SDL_DestroyTexture(text_texture);
    return 0;
}

int Render() {
    SDL_SetRenderDrawColor(rend, 135, 206, 250, 255);
    SDL_RenderClear(rend);
    for(int i = 0; i < land_len; i++) {
        SDL_RenderCopy(rend, land[i].tex, NULL, &land[i].rect);
    }
    for(int i = 0; i < NO_CACTUS; i++) {
        SDL_RenderCopy(rend, cactus[i].tex, NULL, &cactus[i].rect);
    }
    SDL_RenderCopy(rend, boy.tex[(int)current_frame], NULL, &boy.rect);
    RenderScore();
    if(game_state == ENDGAME) {
        RenderText("You Lost!");
    }
    if(game_state == TITLE) {
        RenderText("Space to Start");
    }
    SDL_RenderPresent(rend);
    return 0;
}

int RenderText(char* str) {
    //TODO just being lazy correct it...
    SDL_Surface* sur = TTF_RenderText_Solid(font, str, black_color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(rend, sur);
    int w, h;
    SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);
    w *= 2;
    h *= 2;
    SDL_Rect dest_rect = {.x = (SIZEX/2)-(w/2), .y = (SIZEY/2)-(h/2), .w = w, .h = h};
    SDL_RenderCopy(rend, text_texture, NULL, &dest_rect);
    //SDL_RenderPresent(rend);
    SDL_FreeSurface(sur);
    SDL_DestroyTexture(text_texture);
    return 0;
}

int Free() {
    SDL_DestroyTexture(boy.tex[0]);
    SDL_DestroyTexture(boy.tex[1]);
    SDL_DestroyTexture(land[0].tex);
    SDL_DestroyTexture(cactus[0].tex);
    SDL_DestroyTexture(cactus[1].tex);
    IMG_Quit();
    TTF_Quit();
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(rend);
    SDL_Quit();
    return 0;
}

int main() {
    Init();
    InitGame();
    int quit = 0;
    game_state = TITLE;
    SDL_Event event;
    while(!quit) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;

                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_SPACE) {
                        if(game_state == RUNNING) {
                        vely = boy.rect.y == ground - TILE_SIZE? 15 : vely; //TODO change jump velocity here
                        } else if(game_state == TITLE) {
                            game_state = RUNNING;
                        } else if(game_state == ENDGAME) {

                        }
                    }
                    break;

                default: {}
            }
        }

        switch(game_state) {
            case TITLE:
                Render();
                break;

            case ENDGAME:
                Render();
                break;

            case RUNNING:
                Update();
                Render();
                for(int i = 0; i < NO_CACTUS; i++) {
                    if(check_collison(boy.collison_rect, cactus[i].collison_rect)) {
                        game_state = ENDGAME;
                    }
                }
                break;

            default: {}
        }
        SDL_Delay(16);
    }

    Free();
    return 0;
}
