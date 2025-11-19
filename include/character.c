#include <GL/gl.h>
#include <winuser.h>
#include <stdbool.h>

#include "../headers/character.h"
#include "../headers/texturing.h"

const int WINDOW_HEIGHT = 840;
#define NUM_ANIMATIONS 3
#define SHEET_COLS 18

static const int FRAMES_PER_ANIM[NUM_ANIMATIONS + 1] = {
    0,
    6, // 1 = idle
    12, // 2 = run
    18  // 3 = jump
};

void setGravity(Character *chrt)
{
    
    if (chrt->velocityY < 0)
    {
        chrt->inAir = true;
    }
    if (chrt->inAir == false)
    {
        chrt->velocityX *= 0.8f;              //???????????
        chrt->velocityY *= 0.8f;
    }

    chrt->posX += chrt->velocityX;         //??????????? ???????? ? ????????? ?????????
    chrt->posY += chrt->velocityY;

    chrt->velocityY -= 9.8f;    
}

Character* initCharacter(float x, float y, unsigned int sprite) {
    Character* character = malloc(sizeof(Character));

    character->posX = x;
    character->posY = y;
    character->width = 100.0f;
    character->height = 100.0f;

    character->spriteSheet = sprite;

    character->turnedAround = false;
    character->inAir = false; // стартуем "на земле"
    character->animation = ANIM_IDLE; // по умолчанию -- idle
    character->frame = 0;
    character->velocityX = 0;
    character->velocityY = 0;

    return character;
};

void drawCharacter(Character *chrt) {
    float characterPosX = chrt->posX - chrt->width * 0.5f;
    float characterPosY = WINDOW_HEIGHT - chrt->posY - chrt->height;

    int anim = chrt->animation;
    int framesInAnim = (anim >= 0 && anim <= NUM_ANIMATIONS) ? FRAMES_PER_ANIM[anim] : 1;
    if (framesInAnim <= 0) framesInAnim = 1;

    static const int ANIM_ROW[NUM_ANIMATIONS + 1] = {
        0, // unused
        0, // 1 = idle -> строка 0 (верхняя)
        1, // 2 = run  -> строка 1
        2  // 3 = jump -> строка 2
    };
    int row = (anim >= 0 && anim <= NUM_ANIMATIONS) ? ANIM_ROW[anim] : 0;

    // Защита: если кадр вышел за пределы — сбросим
    if (chrt->frame >= framesInAnim) chrt->frame = 0;
    if (chrt->frame < 0) chrt->frame = 0;

    // использовать шаг по U равный 1 / максимальное число колонок в листе
    float uStep = 1.0f / (float)SHEET_COLS;
    float vStep = 1.0f / (float)NUM_ANIMATIONS;

    // смещаем по колонке, но wrap по реальному числу кадров framesInAnim
    float u0 = (float)(chrt->frame) * uStep;
    float u1 = u0 + uStep;

    // v: вычисляем по ряду. Здесь предполагается что row=0 — верхняя строка изображения.
    // Для OpenGL (v=0 внизу) верхняя строка имеет большие v значения:
    float v0 = 1.0f - (row + 1) * vStep; // нижняя граница строки
    float v1 = 1.0f - row * vStep;       // верхняя граница строки

    float vertices[] = {
        characterPosX + chrt->width, characterPosY + chrt->height, 0.0f,
        1.0f,1.0f,1.0f,
        u1, v1,

        characterPosX + chrt->width, characterPosY, 0.0f,
        1.0f,1.0f,1.0f,
        u1, v0,

        characterPosX, characterPosY, 0.0f,
        1.0f,1.0f,1.0f,
        u0, v0,

        characterPosX, characterPosY + chrt->height, 0.0f,
        1.0f,1.0f,1.0f,
        u0, v1
    };

    if (chrt->turnedAround) {
        vertices[0] = vertices[8] -= chrt->width;
        vertices[16] = vertices[24] += chrt->width;
    }

    renderImageFromMatrix(vertices, chrt->spriteSheet);
}

void changeAnimation(Character *chrt, int animNum) {
    if (chrt->animation != animNum) {
        chrt->animation = animNum;
        chrt->frame = 0;
    }
}

void addVelocity(Character *chrt, float horizontal, float vertical)
{
    chrt->velocityX += horizontal;
    chrt->velocityY += vertical;
}

void moveController (Character *chrt) { //
    if (chrt->inAir) {
        changeAnimation(chrt, ANIM_JUMP);
    }
    else if (GetKeyState(VK_LEFT) < 0) {
        addVelocity(chrt, -10.0f, 0.0f);
        chrt->turnedAround = true;
        changeAnimation(chrt, ANIM_RUN);
    }
    else if (GetKeyState(VK_RIGHT) < 0) {
        addVelocity(chrt, 10.0f, 0.0f);
        chrt->turnedAround = false;
        changeAnimation(chrt, ANIM_RUN);
    }
    else {
        changeAnimation(chrt, ANIM_IDLE);
    }

    // прыжок/спуск как раньше
    if (GetKeyState(VK_UP) < 0 && chrt->inAir != true) {
        addVelocity(chrt, 0.0f, 80.0f);
        chrt->inAir = true;
    }
    if (GetKeyState(VK_DOWN) < 0 && chrt->inAir) {
        addVelocity(chrt, 0.0f, -100.0f);
    }
}

// Заменяем старый updateFrame на персонифицированный апдейт
void updateCharacterFrame(Character *chrt) {
    int anim = chrt->animation;
    int framesInAnim = (anim >= 0 && anim <= NUM_ANIMATIONS) ? FRAMES_PER_ANIM[anim] : 1;
    if (framesInAnim > 0) {
        chrt->frame = (chrt->frame + 1) % framesInAnim;
    }
}

// при рендере персонажа:
static void drawCharacterAt(float x, float y, float w, float h, unsigned int texture, float screenH, int frame, int frames) {
    float verts[32];
    buildSpriteVertices(w, h, x, y, frame, frames, verts, 0, screenH);
    renderImageFromMatrix(verts, texture);
}
