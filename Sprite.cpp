#include "Sprite.h"

Sprite::Sprite() {
    texture = NULL;
    useSourceRect = false;
    origin = D3DXVECTOR2(0.0f, 0.0f);
    SetRectEmpty(&sourceRect);
}

Sprite::Sprite(LPDIRECT3DTEXTURE9 tex) {
    texture = tex;
    useSourceRect = false;
    origin = D3DXVECTOR2(0.0f, 0.0f);
    SetRectEmpty(&sourceRect);
}

void Sprite::CenterOrigin() {
    float w = 0.0f;
    float h = 0.0f;

    if (useSourceRect) {
        // Centre of the current animation cell.
        w = (float)(sourceRect.right - sourceRect.left);
        h = (float)(sourceRect.bottom - sourceRect.top);
    }
    else if (texture) {
        // Centre of the whole texture.
        D3DSURFACE_DESC desc;
        if (SUCCEEDED(texture->GetLevelDesc(0, &desc))) {
            w = (float)desc.Width;
            h = (float)desc.Height;
        }
    }

    origin = D3DXVECTOR2(w / 2.0f, h / 2.0f);
}

D3DXMATRIX Sprite::BuildTransform(D3DXVECTOR2 position, D3DXVECTOR2 scale, float rotation) {
    D3DXMATRIX originMat, scaleMat, rotMat, transMat;

    // Move the pivot to the drawing origin first, then scale, rotate, and place.
    D3DXMatrixTranslation(&originMat, -origin.x, -origin.y, 0.0f);
    D3DXMatrixScaling(&scaleMat, scale.x, scale.y, 1.0f);
    D3DXMatrixRotationZ(&rotMat, rotation);
    D3DXMatrixTranslation(&transMat, position.x, position.y, 0.0f);

    return originMat * scaleMat * rotMat * transMat;
}

void Sprite::Draw(Graphics* graphics, Camera* camera, D3DXVECTOR2 position,
    D3DXVECTOR2 scale, float rotation, D3DCOLOR color) {
    if (!graphics || !texture) return;

    D3DXMATRIX finalMatrix = BuildTransform(position, scale, rotation);

    // Multiply by the camera so the sprite scrolls with the world.
    if (camera) {
        finalMatrix = finalMatrix * camera->GetViewMatrix();
    }

    RECT* src = useSourceRect ? &sourceRect : NULL;
    graphics->DrawSprite(texture, src, &finalMatrix, color);
}

void Sprite::DrawScreen(Graphics* graphics, D3DXVECTOR2 position,
    D3DXVECTOR2 scale, float rotation, D3DCOLOR color) {
    // A screen-space draw is just a world draw with no camera applied.
    Draw(graphics, NULL, position, scale, rotation, color);
}
