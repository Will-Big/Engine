#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <vector>
#include <ctime>
#include <cstdlib>

const int SCREEN_WIDTH = 300;
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
    int pivotIndex; // Index of the pivot block for rotation
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
        tetromino.pivotIndex = -1; // No rotation for square
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

bool checkCollision(const Tetromino& tetromino, const std::vector<std::vector<int>>& board)
{
    for (const Block& block : tetromino.blocks)
    {
        if (block.x < 0 || block.x >= BOARD_WIDTH || block.y >= BOARD_HEIGHT)
            return true;
        if (block.y >= 0 && board[block.y][block.x])
            return true;
    }
    return false;
}

void placeTetromino(const Tetromino& tetromino, std::vector<std::vector<int>>& board)
{
    for (const Block& block : tetromino.blocks)
    {
        if (block.y >= 0)
            board[block.y][block.x] = 1;
    }
}

void clearFullLines(std::vector<std::vector<int>>& board)
{
    for (int y = BOARD_HEIGHT - 1; y >= 0; --y)
    {
        bool isFull = true;
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            if (board[y][x] == 0)
            {
                isFull = false;
                break;
            }
        }

        if (isFull)
        {
            // Clear the line and move everything above down
            for (int row = y; row > 0; --row)
            {
                for (int x = 0; x < BOARD_WIDTH; ++x)
                {
                    board[row][x] = board[row - 1][x];
                }
            }

            // Clear the top line
            for (int x = 0; x < BOARD_WIDTH; ++x)
            {
                board[0][x] = 0;
            }

            // Since we cleared a line, we need to check the same row again
            ++y;
        }
    }
}

void renderBoard(SDL_Renderer* renderer, const std::vector<std::vector<int>>& board)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            if (board[y][x])
            {
                SDL_Rect rect = { x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

void dropTetromino(Tetromino& tetromino, const std::vector<std::vector<int>>& board)
{
    Tetromino droppedTetromino = tetromino;
    while (!checkCollision(droppedTetromino, board))
    {
        for (auto& block : droppedTetromino.blocks)
        {
            block.y++;
        }
    }
    // Move back up one step to avoid collision
    for (auto& block : droppedTetromino.blocks)
    {
        block.y--;
    }
    tetromino = droppedTetromino;
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

void renderGhostTetromino(SDL_Renderer* renderer, Tetromino tetromino, const std::vector<std::vector<int>>& board)
{
    // Drop the tetromino to the expected landing position
    dropTetromino(tetromino, board);

    // Render the ghost tetromino with a semi-transparent color
    SDL_SetRenderDrawColor(renderer, tetromino.color.r / 2, tetromino.color.g / 2, tetromino.color.b / 2, tetromino.color.a);
    for (const Block& block : tetromino.blocks)
    {
        SDL_Rect rect = { block.x * BLOCK_SIZE, block.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }
}

void rotateTetromino(Tetromino& tetromino)
{
    if (tetromino.pivotIndex == -1)
        return; // No rotation for square

    Block pivot = tetromino.blocks[tetromino.pivotIndex];
    for (auto& block : tetromino.blocks)
    {
        int relativeX = block.x - pivot.x;
        int relativeY = block.y - pivot.y;

        // Apply 90-degree rotation transformation
        int rotatedX = -relativeY;
        int rotatedY = relativeX;

        block.x = pivot.x + rotatedX;
        block.y = pivot.y + rotatedY;
    }
}

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned int>(time(0)));

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL could not be initialized! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Simple Tetris Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::vector<std::vector<int>> board(BOARD_HEIGHT, std::vector<int>(BOARD_WIDTH, 0));
    Tetromino currentTetromino = createTetromino();

    bool isRunning = true;
    SDL_Event event;
    Uint32 lastTick = SDL_GetTicks();

    // Game loop
    while (isRunning)
    {
        // Handle events
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                Tetromino movedTetromino = currentTetromino;
                switch (event.key.keysym.sym)
                {
                case SDLK_LEFT:
                    for (auto& block : movedTetromino.blocks)
                        block.x--;
                    if (!checkCollision(movedTetromino, board))
                        currentTetromino = movedTetromino;
                    break;
                case SDLK_RIGHT:
                    for (auto& block : movedTetromino.blocks)
                        block.x++;
                    if (!checkCollision(movedTetromino, board))
                        currentTetromino = movedTetromino;
                    break;
                case SDLK_DOWN:
                    for (auto& block : movedTetromino.blocks)
                        block.y++;
                    if (!checkCollision(movedTetromino, board))
                        currentTetromino = movedTetromino;
                    break;
                case SDLK_UP: // Rotate
                    rotateTetromino(movedTetromino);
                    if (!checkCollision(movedTetromino, board))
                        currentTetromino = movedTetromino;
                    break;
                case SDLK_SPACE: // Drop
                    dropTetromino(currentTetromino, board);
                    break;
                }
            }
        }

        // Update the game state
        Uint32 currentTick = SDL_GetTicks();
        if (currentTick - lastTick > 500)
        {
            Tetromino movedTetromino = currentTetromino;
            for (auto& block : movedTetromino.blocks)
                block.y++;

            if (checkCollision(movedTetromino, board))
            {
                placeTetromino(currentTetromino, board);
                clearFullLines(board);
                currentTetromino = createTetromino();
                if (checkCollision(currentTetromino, board))
                {
                    // Game Over
                    isRunning = false;
                }
            }
            else
            {
                currentTetromino = movedTetromino;
            }
            lastTick = currentTick;
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
        SDL_RenderClear(renderer);

        // Render the board
        renderBoard(renderer, board);

        // Render the ghost tetromino
        renderGhostTetromino(renderer, currentTetromino, board);

        // Render the current tetromino
        renderTetromino(renderer, currentTetromino);

        // Update the screen
        SDL_RenderPresent(renderer);
    }

    // Clean up and quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
