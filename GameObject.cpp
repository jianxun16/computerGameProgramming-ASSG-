#include "GameObject.h"

GameObject::GameObject() {
    position = D3DXVECTOR2(0.0f, 0.0f);
    scale = D3DXVECTOR2(1.0f, 1.0f);
    rotation = 0.0f;
    spriteWidth = 0;
    spriteHeight = 0;
    texture = nullptr;
    isAnimated = false;
    active = true;
}

void GameObject::Initialize(LPDIRECT3DTEXTURE9 tex, D3DXVECTOR2 startPos, int width, int height) {
    texture = tex;
    position = startPos;
    spriteWidth = width;
    spriteHeight = height;
}

void GameObject::SetupAnimation(int texWidth, int texHeight, int cols, int rows, int frames, float speed, CycleDirection dir) {
    animator.Initialize(texWidth, texHeight, cols, rows, frames, speed, dir);

    spriteWidth = texWidth / cols;
    spriteHeight = texHeight / rows;

    isAnimated = true;
}

void GameObject::UpdateLogic(float deltaTime) {
    if (!active) return;

    if (isAnimated) {
        animator.Update(deltaTime);
    }
}

void GameObject::RenderFrame(Graphics* graphics) {
    if (!active || !texture) return;

    // calculate center & set center
    D3DXVECTOR2 spriteCenter(spriteWidth / 2.0f, spriteHeight / 2.0f);
    D3DXVECTOR2 drawPos = position - D3DXVECTOR2(spriteCenter.x * scale.x, spriteCenter.y * scale.y);
    D3DXMATRIX matrix;
    D3DXMatrixTransformation2D(&matrix, NULL, 0.0f, &scale, &spriteCenter, rotation, &drawPos);

    if (isAnimated) {
        RECT srcRect = animator.GetSourceRect();
        graphics->DrawSprite(texture, &srcRect, &matrix);
    }
    else {
        graphics->DrawSprite(texture, NULL, &matrix);
    }
}