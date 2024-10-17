#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[])
{
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

        // 간단한 색상 사각형 그리기 (임시적인 게임 객체 예시)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect fillRect = { SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT / 2 - 25, 50, 50 };
        SDL_RenderFillRect(renderer, &fillRect);

        // 화면 업데이트
        SDL_RenderPresent(renderer);
    }

    // 리소스 해제 및 SDL 종료
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
