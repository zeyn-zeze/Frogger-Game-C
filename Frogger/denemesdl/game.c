//FROGGER OYUNU//
//ZEYNEP OZCELİK 220229034
//Kurbaganın araba ve odunlara carpmadan evine gitmesini hedefleyen bir oyundur.

#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>

//Oyunun nesneleri için tanımlamalar yapılmıştır
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FROG_WIDTH 30
#define FROG_HEIGHT 30
#define FROG_SPEED 5
#define CAR_WIDTH 60
#define CAR_HEIGHT 35
#define CAR1_SPEED 3
#define CAR2_SPEED 2
#define TREE_WIDTH 60
#define TREE_HEIGHT 30
#define TREE_GAP 10
#define TREE_SPEED_LEFT 2
#define TREE_SPEED_RIGHT 1
#define HOUSE_WIDTH 30
#define HOUSE_HEIGHT 30


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* frogTexture = NULL;
SDL_Texture* car1Texture = NULL;
SDL_Texture* car2Texture = NULL;
SDL_Texture* startTexture = NULL;
SDL_Texture* playTexture = NULL;
SDL_Texture* roadTexture = NULL;
SDL_Texture* gameOverTexture = NULL;
SDL_Texture* treeTexture = NULL;
SDL_Texture* houseTextures[5]; // Ev resimlerini tutmak için dizi
SDL_Texture* loadTexture(const char* path);

typedef struct {
    int x;
    int y;
} Frog;

typedef struct {
    int x;
    int y;
    int speed;
    int gap;
} Car;

typedef struct {
    int x;
    int y;
    int speed;
} Tree;

Frog frog;
Car carLeft[5];
Car carRight[5];
Car carTop[5];
Car carBottom[5];
Tree trees[3][5];


enum GameState {

    START,
    PLAYING,
    GAME_OVER,
    GAME_WIN
};
 //Oyun başlangıç,oynama,son olmak üzere 3 bölümden oluşmaktadır

enum GameState gameState = START;

//Fonskiyonlar
void initializeSDL();
void closeSDL();
void handleMouseClick(SDL_Event event);
void handleInput();
void moveCarLeft(Car* car);
void moveCarRight(Car* car);
void moveCar();
void moveTrees();
int checkCollision();
int checkWin();
void renderStartScreen();
void renderGame();
void renderGameOver();



int main(int argc, char* args[]) {

    initializeSDL();//Pencereyi aç

    //Kurbaganın konumunu belirle
    frog.x = (SCREEN_WIDTH - FROG_WIDTH) / 2;
    frog.y = SCREEN_HEIGHT - FROG_HEIGHT;

//Aarabanın konumunu,hızını,aralığını belirle
for (int i = 0; i < 5; i++) {
    carLeft[i].x = SCREEN_WIDTH + (i * (SCREEN_WIDTH / 5));
    carLeft[i].y = (SCREEN_HEIGHT / 2) + (4 * CAR_HEIGHT);
    carLeft[i].speed = CAR1_SPEED;
    carLeft[i].gap = 100;

    carRight[i].x = -CAR_WIDTH - (i * (SCREEN_WIDTH / 5)) - carRight[i].gap;
    carRight[i].y = (SCREEN_HEIGHT / 2) + ( 1.5* CAR_HEIGHT);
    carRight[i].speed = CAR2_SPEED;
    carRight[i].gap = 100;
}

//Resimleri yükle
    frogTexture = loadTexture("game_images/lives.bmp");
    treeTexture = loadTexture("game_images/log.bmp");
    car1Texture = loadTexture("game_images/car1.bmp");
    car2Texture = loadTexture("game_images/car2.bmp");
    startTexture = loadTexture("game_images/startscreen.bmp");
    playTexture = loadTexture("game_images/button2.bmp");
    gameOverTexture = loadTexture("game_images/gameover.bmp");


    //Ağaç konumlarını ve hızlarını başlat
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 5; col++) {
            Tree* tree = &trees[row][col];
            tree->y = (row + 1) * (SCREEN_HEIGHT / 7) + (TREE_HEIGHT /2);  // Ekranın üst yarısına yerleştirilmesi için / 4 yerine / 8 kullanılıyor
            tree->speed = (row % 2 == 0) ? TREE_SPEED_LEFT : TREE_SPEED_RIGHT;
            if (row % 2 == 0) {
                tree->x = (col * (SCREEN_WIDTH / 5)) + 10* TREE_GAP;
            } else {
                tree->x = SCREEN_WIDTH - ((col + 1) * (SCREEN_WIDTH / 5)) - TREE_GAP - TREE_WIDTH;
            }
        }
    }


    while (1) {
        handleInput();

        if (gameState == START) {

            renderStartScreen();
        }
        else if (gameState == PLAYING) {

            moveCar();
            moveTrees();
            SDL_Delay(20);

            if (checkCollision()) {

                gameState = GAME_OVER;
            }

            renderGame();  //Oyunu işle
        }
                 //Kurbaganın eve gelmesi durumunu işle
        if (checkWin()) {
        gameState = GAME_WIN;
        printf("Tebrikler Oyunu Kazandin!\n");
        SDL_Delay(2000); // 2 saniye bekleme

        }

        else if (gameState == GAME_OVER) {
            renderGameOver();
            printf("Uzgunum Oyunu Kaybettin!\n");
            SDL_Delay(2000); // 2000 milisaniyelik gecikme (2 saniye)
            break;
        }
    }

    closeSDL(); //Pencereyi kapat

    return 0;
}



void initializeSDL() {

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Frogger", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    roadTexture = loadTexture("game_images/background2.bmp");//arkaplan

     for(int i = 0 ;i<5 ;i++)
     {
          houseTextures[i] = loadTexture("game_images/ev.bmp");//ev resmi
     }

}

void closeSDL() {

    SDL_DestroyTexture(frogTexture);
    SDL_DestroyTexture(car1Texture);
    SDL_DestroyTexture(car2Texture);
    SDL_DestroyTexture(startTexture);
    SDL_DestroyTexture(playTexture);
    SDL_DestroyTexture(roadTexture);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(treeTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = SDL_LoadBMP(path);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void handleMouseClick(SDL_Event event) //Başlangıç ekranındaki butonun boyutunu ve fareyle birlikteki işlevlerini belirten fonksiyon
{

    if (gameState == START && event.button.clicks == SDL_BUTTON_LEFT) {

        int mouseX = event.button.x;
        int mouseY = event.button.y;

        int playButtonX = (SCREEN_WIDTH - 200) / 2;
        int playButtonY = (SCREEN_HEIGHT - 200) / 2;
        int playButtonWidth = 200;
        int playButtonHeight = 100;

        if (mouseX >= playButtonX && mouseX <= playButtonX + playButtonWidth &&
            mouseY >= playButtonY && mouseY <= playButtonY + playButtonHeight) {
            gameState = PLAYING;
        }
    }

}

void handleInput() {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            closeSDL();
            exit(0);
        }
        else if (event.type == SDL_KEYDOWN) {
            if (gameState == START && event.key.keysym.sym == SDLK_RETURN) {
                gameState = PLAYING;
            }
            else if (gameState == PLAYING) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (frog.y > 0)
                            frog.y -= FROG_SPEED;
                        break;
                    case SDLK_DOWN:
                        if (frog.y < SCREEN_HEIGHT - FROG_HEIGHT)
                            frog.y += FROG_SPEED;
                        break;
                    case SDLK_LEFT:
                        if (frog.x > 0)
                            frog.x -= FROG_SPEED;
                        break;
                    case SDLK_RIGHT:
                        if (frog.x < SCREEN_WIDTH - FROG_WIDTH)
                            frog.x += FROG_SPEED;
                        break;
                }
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            handleMouseClick(event);
        }
    }
}


void moveCarLeft(Car* car) {

    car->x -= car->speed;
    if (car->x + CAR_WIDTH < 0) {
        car->x = SCREEN_WIDTH;
        car->gap = rand() % 5 + 100;
    }
}


void moveCarRight(Car* car) {

    car->x += car->speed;
    if (car->x > SCREEN_WIDTH) {
        car->x = -CAR_WIDTH;
        car->gap = rand() % 5 + 100;
    }
}


void moveCar() {

    for (int i = 0; i < 5; i++) {

        moveCarLeft(&carLeft[i]);
        moveCarRight(&carRight[i]);
    }

}



void moveTrees() {

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 5; col++) {
            Tree* tree = &trees[row][col];
            if (row % 2 == 0) {
                // Agacları sağdan sola hareket ettir
                tree->x -= tree->speed;
                if (tree->x + TREE_WIDTH < 0) {
                    tree->x = SCREEN_WIDTH;
                }
            } else {
                //Agacları soldan saga hareket ettir
                tree->x += tree->speed;
                if (tree->x > SCREEN_WIDTH) {
                    tree->x = -TREE_WIDTH;
                }
            }
        }
    }
}



int checkCollision() {

    int frogLeft = frog.x;
    int frogRight = frog.x + FROG_WIDTH;
    int frogTop = frog.y;
    int frogBottom = frog.y + FROG_HEIGHT;

    for (int i = 0; i < 5; i++) {

        int carLeftLeft = carLeft[i].x;
        int carLeftRight = carLeft[i].x + CAR_WIDTH;
        int carLeftTop = carLeft[i].y;
        int carLeftBottom = carLeft[i].y + CAR_HEIGHT;

        int carRightLeft = carRight[i].x;
        int carRightRight = carRight[i].x + CAR_WIDTH;
        int carRightTop = carRight[i].y;
        int carRightBottom = carRight[i].y + CAR_HEIGHT;

        if ((frogLeft < carLeftRight && frogRight > carLeftLeft && frogTop < carLeftBottom && frogBottom > carLeftTop) ||
            (frogLeft < carRightRight && frogRight > carRightLeft && frogTop < carRightBottom && frogBottom > carRightTop)) {

            return 1; //Carpisma meydana geldi
        }
    }


        for (int row = 0; row < 3; row++)
         {
        for (int col = 0; col < 5; col++)
         {
            Tree* tree = &trees[row][col];
            int treeLeft = tree->x;
            int treeRight = tree->x + TREE_WIDTH;
            int treeTop = tree->y;
            int treeBottom = tree->y + TREE_HEIGHT;

            if (frogLeft < treeRight && frogRight > treeLeft && frogTop < treeBottom && frogBottom > treeTop) {
                return 1; //Carpisma meydana geldi
            }
        }
    }

    return 0; // Carpisma yok

}


int checkWin() {

    int frogTop = frog.y;

    // İlk 5 ev için kontrol yapılıyor

    for (int i = 0; i < 5; i++) {
        int houseTop = (SCREEN_HEIGHT / 7) - (HOUSE_HEIGHT / 1.1);

        // Kurbağa evin içerisine girdiyse oyun kazanılır
        if (frogTop <= houseTop) {
            return 1; // Kazanma durumu
        }

        houseTop -= (i + 1) * (SCREEN_HEIGHT / 7);
    }

    return 0; // Kazanma durumu yok
}


void renderStartScreen() {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect startRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(renderer, startTexture, NULL, &startRect);

    SDL_Rect playRect = { (SCREEN_WIDTH - 200) / 2, (SCREEN_HEIGHT - 100) / 2, 200, 100 };
    SDL_RenderCopy(renderer,playTexture, NULL, &playRect);
    SDL_RenderPresent(renderer);
}


void renderGame() {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect roadRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(renderer, roadTexture, NULL, &roadRect);


     // Agacları işle
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 5; col++) {
            Tree* tree = &trees[row][col];
            SDL_Rect treeRect = { tree->x, tree->y, TREE_WIDTH, TREE_HEIGHT };
            SDL_RenderCopy(renderer, treeTexture, NULL, &treeRect);
        }
    }


      //Arabaları işle
    for (int i = 0; i < 5; i++) {
        SDL_Rect carLeftRect = { carLeft[i].x, carLeft[i].y, CAR_WIDTH, CAR_HEIGHT };
        SDL_RenderCopy(renderer, car1Texture, NULL, &carLeftRect);

        SDL_Rect carRightRect = { carRight[i].x, carRight[i].y, CAR_WIDTH, CAR_HEIGHT };
        SDL_RenderCopy(renderer, car2Texture, NULL, &carRightRect);
    }


//Kurbagayi işle
    SDL_Rect frogRect = { frog.x, frog.y, FROG_WIDTH, FROG_HEIGHT };
    SDL_RenderCopy(renderer, frogTexture, NULL, &frogRect);


    // Evleri isle
    for (int i = 0; i < 5; i++) {
        int houseX = (i* (SCREEN_WIDTH / 5)) + (SCREEN_WIDTH /10) - (HOUSE_WIDTH / 2);
        int houseY = (SCREEN_HEIGHT / 7) - (HOUSE_HEIGHT /1.1);
        SDL_Rect houseRect = { houseX, houseY, 1.1*HOUSE_WIDTH, HOUSE_HEIGHT };
        SDL_RenderCopy(renderer, houseTextures[i], NULL, &houseRect);
    }



    SDL_RenderPresent(renderer);


}


void renderGameOver() {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect gameOverRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);

    SDL_RenderPresent(renderer);
}






























