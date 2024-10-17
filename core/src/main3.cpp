#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BLOCK_SIZE = 30;

const int BOARD_WIDTH = SCREEN_WIDTH / BLOCK_SIZE;
const int BOARD_HEIGHT = SCREEN_HEIGHT / BLOCK_SIZE;

struct Block
{
    int x, y;
};

struct Tetromino
{
    std::vector<Block> blocks;
    SDL_Color color;
    int pivotIndex; // 회전을 위한 피벗 블록의 인덱스
};

Tetromino createTetromino()
{
    Tetromino tetromino;
    int type = rand() % 5;
    tetromino.color = { (Uint8)(rand() % 256), (Uint8)(rand() % 256), (Uint8)(rand() % 256), 255 };

    switch (type)
    {
    case 0: // Line
        tetromino.blocks = { {4, 0}, {5, 0}, {6, 0}, {7, 0} };
        tetromino.pivotIndex = 1;
        break;
    case 1: // Square
        tetromino.blocks = { {4, 0}, {5, 0}, {4, 1}, {5, 1} };
        tetromino.pivotIndex = -1; // 회전 없음
        break;
    case 2: // T-shape
        tetromino.blocks = { {4, 0}, {5, 0}, {6, 0}, {5, 1} };
        tetromino.pivotIndex = 1;
        break;
    case 3: // L-shape
        tetromino.blocks = { {4, 0}, {5, 0}, {6, 0}, {4, 1} };
        tetromino.pivotIndex = 1;
        break;
    case 4: // Reverse L-shape
        tetromino.blocks = { {4, 0}, {5, 0}, {6, 0}, {6, 1} };
        tetromino.pivotIndex = 1;
        break;
    }

    return tetromino;
}

void renderTetromino(SDL_Renderer* renderer, const Tetromino& tetromino)
{
    SDL_SetRenderDrawColor(renderer, tetromino.color.r, tetromino.color.g, tetromino.color.b, tetromino.color.a);
    for (const Block& block : tetromino.blocks)
    {
        SDL_Rect rect = { block.x * BLOCK_SIZE, block.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }
}

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned int>(time(0)));

    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL 초기화 실패! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 윈도우 생성
    SDL_Window* window = SDL_CreateWindow("Tetromino", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cerr << "윈도우 생성 실패! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // 렌더러 생성
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "렌더러 생성 실패! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 게임 루프
    bool isRunning = true;
    SDL_Event event;
    Uint32 lastTick = SDL_GetTicks();

    // 초기 테트로미노 생성
    Tetromino currentTetromino = createTetromino();

    while (isRunning)
    {
        // 이벤트 처리
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        // 게임 로직 업데이트
        Uint32 currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        // 화면 지우기 (검은색 배경)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 현재 테트로미노 렌더링
        renderTetromino(renderer, currentTetromino);

        // 화면 업데이트
        SDL_RenderPresent(renderer);
    }

    // 리소스 해제 및 SDL 종료
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
