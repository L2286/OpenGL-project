#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>

#include "headers/menu.h"
#include "headers/character.h"
#include "headers/texturing.h"
#include "headers/collisions.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

const float FPS = 30.0f;
int scene = 0;
const float W = 1440.0f, Y = 840.0f;
const int buttonWidth = 400, buttonHeight = 100;

void changeScene() {
    (scene) ? (scene = 0) : (scene = 1);
}

void exitApp(void) {
    PostQuitMessage(0);
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          W,
                          Y,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    RECT rct;
    GetClientRect(hwnd,&rct);
    glOrtho(0,rct.right, rct.bottom, 0, 1, -1);

    // set menu buttons
    int xPosBtn = W / 2 - buttonWidth / 2;
    int yPosBtn = Y / 2 - buttonHeight / 2 - 100;

    addBtnToMenu("Start game", xPosBtn, yPosBtn, 400, 100, 6, changeScene);
    addBtnToMenu("Exit", xPosBtn, yPosBtn + 150, 400, 100, 6, exitApp);

    // init texture
    unsigned int spriteSheet, background, backgroundMenu, ground;
    initTexture("src/character-3.png", &spriteSheet);
    initTexture("src/background-main.png", &background);
    initTexture("src/background-menu.png", &backgroundMenu);
    initTexture("src/ground.png", &ground);

    // init character
    Character* character = initCharacter(500.0f, 200.0f, spriteSheet);
    printf("%d", spriteSheet);

    // config ground
    const float xSizeGround = 362.0f, ySizeGround = 65.0f;
    #define numberOfGrounds 5
    const float groundsPos[numberOfGrounds][2];

    // config colliders
    #define numbersOfCollider 4
    const float colliders[numbersOfCollider][4] = {
        {-100.0f, 1440.0f, 0, 150.0f},
        {-200.0f, 0.0f, 0.0f, 1200.0f},
        {1440.0f, 1460.0f, 0.0f, 1200.0f}
    };

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */
            switch (scene)
            {
                case 0:
                    renderImage(W, Y, 0, 0, backgroundMenu, 0, Y);
                    showMenu();
                    break;
                case 1:
                    // логика обновления
                    moveController(character);
                    setGravity(character);
                    updateCharacterFrame(character);
                    // проверяем столкновения до рендера, чтобы позиция была актуальна
                    collisionCheck(character, xSizeGround, ySizeGround, groundsPos, numberOfGrounds, colliders, numbersOfCollider);

                    renderImage(W, Y, 0, 0, background, 0, Y);
                    for (int i = 0; i < numberOfGrounds; i++)
                        renderImage(xSizeGround, ySizeGround, groundsPos[i][0], groundsPos[i][1], ground, 1, Y);

                    // рендерим персонажа поверх фона/платформ
                    drawCharacter(character);
                    // после updateCharacterFrame(character);
                    printf("anim=%d inAir=%d frame=%d posY=%.1f velY=%.1f\n",
                           character->animation, character->inAir, character->frame,
                           character->posY, character->velocityY);
                    break;
            }

            SwapBuffers(hDC);

            theta += 1.0f;
            Sleep (1 + FPS);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_MOUSEMOVE:
            mouseMoveMenu(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_LBUTTONDOWN:
            mouseDownMenu();
        break;

        case WM_LBUTTONUP:
            mouseUpMenu();
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    changeScene();
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
