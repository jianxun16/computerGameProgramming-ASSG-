#include "Camera.h"

Camera::Camera(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    position = D3DXVECTOR2(0.0f, 0.0f);
    D3DXMatrixIdentity(&viewMatrix);
}

void Camera::Update() {
    // move world opp to cam and center cam on monitor
    D3DXMatrixTranslation(&viewMatrix,
        -position.x + (screenWidth / 2.0f),
        -position.y + (screenHeight / 2.0f),
        0.0f);
}

// capture mouse click relative to world pos
D3DXVECTOR2 Camera::ScreenToWorld(D3DXVECTOR2 screenPos) {
    return D3DXVECTOR2(screenPos.x + position.x - (screenWidth / 2.0f),
        screenPos.y + position.y - (screenHeight / 2.0f));
}