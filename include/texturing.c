#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include "../headers/texturing.h"
#include <string.h> // для memcpy

#define STB_IMAGE_IMPLEMENTATION
#include "../stb-master/stb_image.h"

void initTexture(char* pathToImg, unsigned int *texture) {
    int width, height, nrChannels;

    unsigned char *data  = stbi_load(pathToImg, &width, &height, &nrChannels, 0);
    printf("%i %i %i\n", width, height, nrChannels);

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_RGBA, 
        width, height, 
        0, 
        nrChannels == 4? GL_RGBA : GL_RGB, 
        GL_UNSIGNED_BYTE, 
        data);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
}

void _render(float vertices[], unsigned int texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);

    glPushMatrix();
    // -
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 8 * sizeof(float), vertices);
    glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(float), vertices + 6);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //-
    glPopMatrix();
    glDisable(GL_ALPHA_TEST);
}

void renderImage(float width, float height, float xPos, float yPos, unsigned int texture, int isGround, float screenHight) {
    if (isGround) {
        float vertices[] = {
            xPos + width, screenHight - yPos - height, 0.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f,

            xPos + width, screenHight - yPos, 0.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 0.0f,

            xPos, screenHight - yPos, 0.0f,
            1.0f, 1.0f, 1.0f,
            0.0f, 0.0f,

            xPos, screenHight - yPos - height, 0.0f,
            1.0f, 1.0f, 1.0f,
            0.0f, -1.0f,
        };

        _render(vertices, texture);
    } 
    else {
        float vertices[] = {
            xPos + width, yPos + height, 0.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f,

            xPos + width, yPos, 0.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 0.0f,

            xPos, yPos, 0.0f,
            1.0f, 1.0f, 1.0f,
            0.0f, 0.0f,

            xPos, yPos + height, 0.0f,
            1.0f, 1.0f, 1.0f,
            0.0f, 1.0f,
        };

        _render(vertices, texture);
    }
}

void renderImageFromMatrix (float vertices[], unsigned int texture)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);

    glPushMatrix();
    // -
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 8 * sizeof(float), vertices);
    glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(float), vertices + 6);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    // -
    glPopMatrix();
    glDisable(GL_ALPHA_TEST);
}

// Новая функция: формирует 4 вертекса (каждый = 8 float) в outVertices
void buildSpriteVertices(float width, float height, float xPos, float yPos,
                         int frame, int frames, float outVertices[32],
                         int isGround, float screenHight)
{
    float fw = 1.0f / (float)frames;
    float u0 = frame * fw;
    float u1 = u0 + fw;
    float v0 = 0.0f;
    float v1 = 1.0f;

    if (isGround) {
        float verts[32] = {
            xPos + width, screenHight - yPos - height, 0.0f,  1.0f,1.0f,1.0f,  u1, v0,
            xPos + width, screenHight - yPos,          0.0f,  1.0f,1.0f,1.0f,  u1, v1,
            xPos,         screenHight - yPos,          0.0f,  1.0f,1.0f,1.0f,  u0, v1,
            xPos,         screenHight - yPos - height, 0.0f,  1.0f,1.0f,1.0f,  u0, v0,
        };
        memcpy(outVertices, verts, sizeof(verts));
    } else {
        float verts[32] = {
            xPos + width, yPos + height, 0.0f,  1.0f,1.0f,1.0f,  u1, v1,
            xPos + width, yPos,          0.0f,  1.0f,1.0f,1.0f,  u1, v0,
            xPos,         yPos,          0.0f,  1.0f,1.0f,1.0f,  u0, v0,
            xPos,         yPos + height, 0.0f,  1.0f,1.0f,1.0f,  u0, v1,
        };
        memcpy(outVertices, verts, sizeof(verts));
    }
}
