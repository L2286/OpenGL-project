#ifndef TEXTURING_H
#define TEXTURING_H
#include "texturing.h"

void initTexture(char* path, unsigned int *texture);
void renderImage(float width, float height, float xPos, float yPos, unsigned int texture, int isGround, float screenHight);
void renderImageFromMatrix (float vertices[], unsigned int texture);
void buildSpriteVertices(float width, float height, float xPos, float yPos,
                         int frame, int frames, float outVertices[32],
                         int isGround, float screenHight);


#endif // TEXTURING_H
