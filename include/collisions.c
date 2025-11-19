#include <GL/gl.h>
#include <stdbool.h>

#include "../headers/character.h"
#include "../headers/collisions.h"

bool _isOverlap(float x11, float x12, float y11, float y12, float x21, float x22, float y21, float y22) {
    return !(x11 > x22 || x12 < x21 || y11 > y22 || y12 < y21);
}

bool _isThereCollisionInNextFrame(float chrtX1, float chrtX2, float chrtY1, float chrtY2, float collX1, float collX2, float collY1, float collY2) {
    return _isOverlap(
        chrtX1, chrtX2, chrtY1, chrtY2, collX1, collX2, collY1, collY2
    );
}

bool _checkBottomCollision(float chrtX1, float chrtX2, float chrtY1, float chrtY2,float colliderX1, float colliderX2, float colliderY1) {
    return _isOverlap(chrtX1, chrtX2, chrtY1, chrtY2, colliderX1, colliderX2, chrtY1, colliderY1);
}

bool _checkUpCollision(float chrtX1, float chrtX2, float chrtY1, float chrtY2,float colliderX1, float colliderX2, float colliderY2) {
    return _isOverlap(chrtX1, chrtX2, chrtY1, chrtY2, colliderX1, colliderX2, colliderY2, chrtY2);
}

bool _checkLeftCollision(float chrtX1, float chrtX2, float chrtY1, float chrtY2,float colliderX2, float colliderY1, float colliderY2) {
    return _isOverlap(chrtX1, chrtX2, chrtY1, chrtY2, colliderX2, chrtX2, colliderY1, colliderY2);
}

bool _checkRightCollision(float chrtX1, float chrtX2, float chrtY1, float chrtY2,float colliderX1, float colliderY1, float colliderY2) {
    return _isOverlap(chrtX1, chrtX2, chrtY1, chrtY2, chrtX1, colliderX1, colliderY1, colliderY2);
}

void _renderColliders(float x1, float x2, float y1, float y2) {
    glPushMatrix();
    // -
        glBegin(GL_LINE_LOOP);
        // --
            glVertex2f(x1, 840 - y1);
            glVertex2f(x1, 840 - y2);
            glVertex2f(x2, 840 - y2);
            glVertex2f(x2, 840 - y1);
        // --
        glEnd();
    // -
    glPopMatrix();
}

void _collisionCheck(Character *chrt, float W, float H, float chrtX1, float chrtX2, float chrtY1, float chrtY2, float colliderX1, float colliderX2, float colliderY1, float colliderY2) {
    bool result = _isThereCollisionInNextFrame(chrtX1 + chrt->velocityX , chrtX2 + chrt->velocityX, chrtY1 + chrt->velocityY, chrtY2 + chrt->velocityY, colliderX1, colliderX2, colliderY1, colliderY2);
    if (result)
    {
        if (_checkUpCollision(chrtX1, chrtX2, chrtY1, chrtY2, colliderX1, colliderX2, colliderY2))
        {
            chrt->velocityY = 0;
            chrt->posY = colliderY2 + 0.1f;
            chrt->inAir = false;
        }
        if (_checkBottomCollision(chrtX1, chrtX2, chrtY1, chrtY2, colliderX1, colliderX2, colliderY1))
        {
            chrt->velocityY = 0;
            chrt->posY = colliderY1 - H - 0.1f;
        }
        if (_checkLeftCollision(chrtX1, chrtX2, chrtY1, chrtY2, colliderX2, colliderY1, colliderY2))
        {
            chrt->velocityX = 0;
            chrt->posX = colliderX2 + W * 0.5f + 0.1f;
        }
        if (_checkRightCollision(chrtX1, chrtX2, chrtY1, chrtY2, colliderX1, colliderY1, colliderY2))
        {
            chrt->velocityX = 0;
            chrt->posX = colliderX1 - W * 0.5f - 0.1f;
        }
    };

    // _renderColliders(colliderX1, colliderX2, colliderY1, colliderY2);   // DEBUG RENDER COLLIDERS
}

void collisionCheck(Character *chrt, float xSizeGround, float ySizeGround, const float groundsCoords[][2], int numberOfGrounds, const float colliders[][4], int numberOfColliders) {
    float characterColliderWidth = chrt->width * 0.5f;
    float characterColliderHeight = chrt->height;

    float chrtX1 = chrt->posX - characterColliderWidth * 0.5f;
    float chrtX2 = chrt->posX + characterColliderWidth * 0.5f;
    float chrtY1 = chrt->posY;
    float chrtY2 = chrt->posY + characterColliderHeight;

    // _renderColliders(chrtX1 + chrt->velocityX, chrtX2 + chrt->velocityX, chrtY1 + chrt->velocityY, chrtY2 + chrt->velocityY);   // DEBUG RENDER CHARACTER COLLIDERS

    for (int i = 0; i < numberOfGrounds; i++) {
        float colliderX1 = groundsCoords[i][0];                         //x1
        float colliderX2 = groundsCoords[i][0] + xSizeGround;           //x2
        float colliderY1 = groundsCoords[i][1];                         //y1
        float colliderY2 = groundsCoords[i][1] + ySizeGround - 25.0f;   //y2
        
        _collisionCheck(
            chrt, characterColliderWidth, characterColliderHeight,
            chrtX1, chrtX2, chrtY1, chrtY2,
            colliderX1, colliderX2, colliderY1, colliderY2
        );
    }

    for (int i = 0; i < numberOfColliders; i++) {
        float colliderX1 = colliders[i][0];                             //x1
        float colliderX2 = colliders[i][1];                             //x2
        float colliderY1 = colliders[i][2];                             //y1
        float colliderY2 = colliders[i][3];                             //y2

        _collisionCheck(
            chrt, characterColliderWidth, characterColliderHeight,
            chrtX1, chrtX2, chrtY1, chrtY2,
            colliderX1, colliderX2, colliderY1, colliderY2
        );
    }
}