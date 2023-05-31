#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include "windows.h"
#include "PCG_Core.h"
#include "WindowRenderer.h"
#include "PCG_Input.h"

#define ROW_COUNT 11
#define COLUMN_COUNT 11
#define STARTING_SNAKE_LENGTH 3

char grid[ROW_COUNT][COLUMN_COUNT] = {0};

PCG_Point *snakePoints;
int snakeLength = STARTING_SNAKE_LENGTH;

int PointEquals(PCG_Point a, PCG_Point b) {
    return ((int) roundf(a.x) == (int) roundf(b.x) && (int) roundf(a.y) == (int) roundf(b.y));
}

int DirectionIsOpp(PCG_Point a, PCG_Point b) {
    if (a.x == -b.x && a.x != 0) {
        return 1;
    }

    if (a.y == -b.y && a.y != 0) {
        return 1;
    }

    return 0;
}

void InitializeSnake() {
    snakePoints = malloc(snakeLength * sizeof(PCG_Point));
    snakePoints[0] = (PCG_Point) {COLUMN_COUNT / 2, ROW_COUNT / 2};

    for (int i = 1; i < snakeLength; i++) {
        snakePoints[i] = (PCG_Point) {(int) snakePoints[i - 1].x, (int) snakePoints[i - 1].y};
    }
}

void RenderWindows(PCG_Point applePos) {
    //Render the actual grid, updating it where necessary
    for (int r = 0; r < ROW_COUNT; r++) {
        for (int c = 0; c < COLUMN_COUNT; c++) {

            PCG_Point currentPoint = (PCG_Point) {c, r};

            //If theres nothign there
            grid[r][c] = ' ';

            //if theres an apple there
            if (PointEquals(currentPoint, applePos)) {
                grid[r][c] = 149;
            }

            //If theres the snake body there
            for (int s = 0; s < snakeLength; s++) {
                if (PointEquals(currentPoint, snakePoints[s])) {
                    grid[r][c] = '#';
                }
            }

            if (grid[r][c] == '#') {
                WT_SetPixel(currentPoint, 1);
            } else if (grid[r][c] == ' ') {
                WT_SetPixel(currentPoint, 0);
            } else {
                WT_SetPixel(currentPoint, 2);
            }
        }
    }
}

int inputChangedThisFrame = 0;
PCG_Point direction;


void Quit() {
    exit(0);
}

/// Shows the death message
/// \param message
void ShowDeathMessage(char *message) {

    //TODO WARNING, this + 4 is for the length of the number, i guesstimate that you likely wont get a score thats greater than 9999, but this may vary based on your game lol

    //Yay c string manipulation
    char *formattedText = (char *) malloc(strlen(message) + 4 + 1);
    sprintf(formattedText, message, snakeLength);

    int choice = MessageBox(NULL, formattedText, "Windows Snake", MB_OKCANCEL | MB_ICONQUESTION);
    free(formattedText);

    // Process the user's choice
    switch (choice) {

        //Replay if the ok button is pressed
        case IDOK : {
            //Restart the exe
            char executablePath[MAX_PATH];
            GetModuleFileName(NULL, executablePath, MAX_PATH);
            ShellExecute(NULL, "open", executablePath, NULL, NULL, SW_SHOWDEFAULT);
            exit(0);
            break;
        }
        case IDCANCEL:
            Quit();
            break;
    }
}

void SetDirUp() {
    if (!DirectionIsOpp(direction, PCG_PointDown) && !inputChangedThisFrame) {
        direction = PCG_PointDown;
        inputChangedThisFrame = 1;
    }
}

void SetDirDown() {
    if (!DirectionIsOpp(direction, PCG_PointUp) && !inputChangedThisFrame) {
        direction = PCG_PointUp;
        inputChangedThisFrame = 1;
    }
}

void SetDirRight() {
    if (!DirectionIsOpp(direction, PCG_PointRight) && !inputChangedThisFrame) {
        direction = PCG_PointRight;
        inputChangedThisFrame = 1;
    }
}

void SetDirLeft() {
    if (!DirectionIsOpp(direction, PCG_PointLeft) && !inputChangedThisFrame) {
        direction = PCG_PointLeft;
        inputChangedThisFrame = 1;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    //Hide the console window
    FreeConsole();

    srand((unsigned int) time(NULL));

    //Initialize the snakedwawdsawdwasdwasasaa
    InitializeSnake();

    //Initialize the window renderer
    WT_InitWindowRenderer(hInstance, ROW_COUNT, COLUMN_COUNT);

    //Input related
    PCG_InitInput();

    //Register the input keys
    PCG_RegisterKeyEvent(KEY_W, &SetDirUp, KE_Down, NULL);
    PCG_RegisterKeyEvent(KEY_A, &SetDirLeft, KE_Down, NULL);
    PCG_RegisterKeyEvent(KEY_S, &SetDirDown, KE_Down, NULL);
    PCG_RegisterKeyEvent(KEY_D, &SetDirRight, KE_Down, NULL);

    PCG_RegisterKeyEvent(KEY_UP, &SetDirUp, KE_Down, NULL);
    PCG_RegisterKeyEvent(KEY_LEFT, &SetDirLeft, KE_Down, NULL);
    PCG_RegisterKeyEvent(KEY_DOWN, &SetDirDown, KE_Down, NULL);
    PCG_RegisterKeyEvent(KEY_RIGHT, &SetDirRight, KE_Down, NULL);

    PCG_RegisterKeyEvent(KEY_Q, &Quit, KE_Down, NULL);
    PCG_RegisterKeyEvent(KEY_ESCAPE, &Quit, KE_Down, NULL);

    //Place the apple
    PCG_Point applePos = {rand() % COLUMN_COUNT, rand() % ROW_COUNT};

    float deltaTime = 1;
    int alive = 1;

    MSG msg;
    while (alive) {

        inputChangedThisFrame = 0;

        var startTime = (float) clock() / CLOCKS_PER_SEC;

        var endTime = (float) clock() / CLOCKS_PER_SEC;
        deltaTime = endTime - startTime;

        //Get the input, we do this numerous times, which wouldnt work in a lot of scenarios but it works here sooo
        //Tweak these values to change the framerate
        for (int i = 0; i < 1000; i++) {
            PCG_InputFrameStart();
            usleep(400);
        }

        //Window message loop
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

            if (msg.message == WM_QUIT) {
                return 1;
            }
            if (msg.message == WM_CLOSE) {
                return 1;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }


        //Move the tail
        for (int i = snakeLength - 1; i > 0; i--) {
            snakePoints[i] = (PCG_Point) {(int) roundf(snakePoints[i - 1].x), (int) roundf(snakePoints[i - 1].y)};
        }


        //Update the head position
        snakePoints[0] = (PCG_Point) {snakePoints[0].x + direction.x,
                                      snakePoints[0].y + direction.y};

        //Check for collisions
        if (snakePoints[0].x < 0 || snakePoints[0].x >= COLUMN_COUNT ||
            snakePoints[0].y < 0 || snakePoints[0].y >= ROW_COUNT) {
            ShowDeathMessage("You hit a wall! Your max length was %d. Try again?");
            alive = 0;
        }

        //If not moving
        if(!PointEquals(direction, PCG_PointZero)){
            for(int s = 1; s < snakeLength; s++){
                if(PointEquals(snakePoints[0], snakePoints[s]))
                {
                    ShowDeathMessage("You hit your own tail! Your max length was %d. Try again?");
                    alive = 0;
                }
            }
        }

        //Picked up apple
        if (PointEquals(applePos, snakePoints[0])) {

            //Increase the snake length
            snakeLength++;
            snakePoints = realloc(snakePoints, snakeLength * sizeof(PCG_Point));

            //We set this position to -1 so it isnt drawn
            snakePoints[snakeLength - 1] = (PCG_Point) {-1, -1};

            //Win condition I guess
            if (snakeLength == ROW_COUNT * COLUMN_COUNT - 1) {
                //printf("You win!!!");
                RenderWindows(applePos);
                ShowDeathMessage("You win!!! Your final tail length was %d!! Play again?");
                alive = 0;
            }

            //We do our best to find a new spot, worst case scenario this is slow but EHHHHHHHHHhhhhhhhhh
            //TODO this no even workie
            int placed = 0;
            while (placed == 0) {
                applePos.y = rand() % ROW_COUNT;
                applePos.x = rand() % COLUMN_COUNT;

                //This is just checking if the apple is anywhere on the snake, if so we try again
                int counter = 0;
                for (int s = 0; s < snakeLength; s++) {
                    if (PointEquals(snakePoints[0], applePos)) {
                        counter++;
                        break;
                    }
                }

                if (counter == 0) {
                    placed = 1;
                }
            }
        }

        RenderWindows(applePos);

        PCG_InputFrameEnd(deltaTime);
    }


    return 0;
}
