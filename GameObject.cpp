#include "GameObject.h"

GameObject::GameObject() {
    position = D3DXVECTOR2(0.0f, 0.0f);
    scale = D3DXVECTOR2(1.0f, 1.0f);
    rotation = 0.0f;
    spriteWidth = 0;
    spriteHeight = 0;
    texture = NULL;
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

    // Feed the current state into the reusable Sprite, then let it build the
    // transform and draw. The pivot is the middle of the frame so scaling and
    // rotation happen around the object's centre.
    sprite.SetTexture(texture);
    if (isAnimated) {
        sprite.SetSourceRect(animator.GetSourceRect());
    }
    else {
        sprite.ClearSourceRect();
    }
    sprite.SetOrigin(spriteWidth / 2.0f, spriteHeight / 2.0f);

    sprite.Draw(graphics, camera, position, scale, rotation);
}