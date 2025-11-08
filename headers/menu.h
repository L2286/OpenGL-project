#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

int addBtnToMenu(
    char *name,
    float x, float y,
    float width, float height,
    float textScale,
    void (*function)()
);
void showMenu();

int mouseMoveMenu(float x, float y);
int mouseDownMenu();
void mouseUpMenu();


#endif // MENU_H_INCLUDED
