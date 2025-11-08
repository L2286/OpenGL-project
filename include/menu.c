#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include "../stb-master/stb_easy_font.h"

#include "../headers/menu.h"

#define nameLen 20

int btnCount = 0;
float mousePosX, mousePosY;

typedef struct
{
    char name[nameLen];
    float vert[8];
    char isHover, isDown, isDone;
    float buffer[50 * nameLen];
    int numQuads;
    float textPosX, textPosY, textScale;

    void (*function)();
} TBtn;

TBtn *btn = 0;

int addBtnToMenu(char *name, float x, float y, float width, float height, float textScale, void (*function)()) {
    btnCount++;
    btn = realloc(btn, sizeof(btn[0]) * btnCount);

    snprintf(btn[btnCount - 1].name, nameLen, "%s", name);

    float *vert = btn[btnCount - 1].vert;
    vert[0] = vert[6] = x;
    vert[2] = vert[4] = x + width;
    vert[1] = vert[3] = y;
    vert[5] = vert[7] = y + height;
    btn[btnCount - 1].isHover = 0;
    btn[btnCount - 1].isDown = 0;
    btn[btnCount - 1].isDone = 0;

    TBtn *b = btn + btnCount - 1;
    b->numQuads = stb_easy_font_print(0,0, name, 0, b->buffer, sizeof(b->buffer));
    b->textPosX = x + (width - stb_easy_font_width(name) * textScale) / 2.0;
    b->textPosY = y + (height - stb_easy_font_height(name) * textScale) / 2.0;;
    b->textPosY += textScale * 2;
    b->textScale = textScale;

    b->function = function;

    return btnCount - 1;
}

void _renderBtn(int btnIdx) {
    TBtn currentBtn = btn[btnIdx];

    glVertexPointer(2, GL_FLOAT, 0, currentBtn.vert);
    glEnableClientState(GL_VERTEX_ARRAY);
    // -
    if (currentBtn.isDone)
        glColor3f(0.81, 0.07, 0.25);
    else if (currentBtn.isHover)
        glColor3f(0.91, 0.66, 0);
    else
        glColor3f(0.6, 0.6, 0.6);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glColor3f(1, 1, 1);
    glLineWidth(1);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    // -
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
    // -
    glColor3f(0, 0, 0);
    glTranslatef(currentBtn.textPosX, currentBtn.textPosY, 0);
    glScalef(currentBtn.textScale, currentBtn.textScale, 1);
    glEnableClientState(GL_VERTEX_ARRAY);
    // --
    glVertexPointer(2, GL_FLOAT, 16, currentBtn.buffer);
    glDrawArrays(GL_QUADS, 0, currentBtn.numQuads * 4);
    // --
    // -
    glPopMatrix();

    if (currentBtn.isDown & !currentBtn.isDone) {
        currentBtn.function(currentBtn.name);
        btn[btnIdx].isDone = 1;
    }
}

void showMenu() {
    for (int i = 0; i < btnCount; i++)
        _renderBtn(i);
}

char _isCoordInButton(int buttonIdx, float x, float y) {
    float *vert = btn[buttonIdx].vert;
    return (x > vert[0]) && (y > vert[1]) && (x < vert[4]) && (y < vert[5]);

}

int mouseMoveMenu(float x, float y) {
    mousePosX = x;
    mousePosY = y;
    int moveBtn = -1;

    for (int i = 0; i < btnCount; i++) {
        if (_isCoordInButton(i, mousePosX, mousePosY)) {
            btn[i].isHover = 1;
            moveBtn = i;
        }
        else {
            btn[i].isHover = 0;
            btn[i].isDown = 0;
        }
    }
    return moveBtn;
}

int mouseDownMenu() {
    int downBtn = -1;
    for (int i = 0; i < btnCount; i++)
        if (_isCoordInButton(i, mousePosX, mousePosY))
        {
            btn[i].isDown = 1;
            downBtn = i;
        }
    return downBtn;
}

void mouseUpMenu() {
    for (int i = 0; i < btnCount; i++) {
        btn[i].isDown = 0;
        btn[i].isDone = 0;
    }
}