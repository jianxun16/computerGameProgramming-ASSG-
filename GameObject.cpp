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

void GameObject::RenderFrame(Graphics* graphics, Camera* camera) {
    if (!active || !texture) return;

    D3DXMATRIX originMat, scaleMat, rotMat, transMat, objectMatrix;

    D3DXMatrixTranslation(&originMat, -(spriteWidth / 2.0f), -(spriteHeight / 2.0f), 0.0f);

    // world transformation
    D3DXMatrixScaling(&scaleMat, scale.x, scale.y, 1.0f);
    D3DXMatrixRotationZ(&rotMat, rotation);
    D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);

    // combine
    objectMatrix = originMat * scaleMat * rotMat * transMat;

    // mutiply by cam matrix, so it fits and will change correctly
    D3DXMATRIX finalMatrix = objectMatrix * camera->GetViewMatrix();

    RECT* sourceRect = nullptr;
    if (isAnimated) {
        RECT frameRect = animator.GetSourceRect();
        sourceRect = &frameRect;
    }

    graphics->DrawSprite(texture, sourceRect, &finalMatrix);
}