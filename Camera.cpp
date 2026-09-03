#include "Camera.h"

Camera::Camera(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    position = D3DXVECTOR2(0.0f, 0.0f);
    D3DXMatrixIdentity(&viewMatrix);
}

void Camera::Update() {
    // World-to-screen is simply "subtract the camera position": a tile at world
    // X appears at (worldX - camX) on screen. The states already centre the view
    // on the player by setting camPos = playerPos - screenSize/2, so the camera
    // must NOT add screenSize/2 again here or every world object (tiles, items,
    // the player) would be pushed half a screen apart from each other.
    D3DXMatrixTranslation(&viewMatrix, -position.x, -position.y, 0.0f);
}

// Inverse of the view transform above: world = screen + camera position.
D3DXVECTOR2 Camera::ScreenToWorld(D3DXVECTOR2 screenPos) {
    return D3DXVECTOR2(screenPos.x + position.x,
        screenPos.y + position.y);
}