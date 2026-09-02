#pragma once
#include <d3dx9.h>

class Camera {
private:
    D3DXVECTOR2 position;
    int screenWidth;
    int screenHeight;
    D3DXMATRIX viewMatrix;

public:
    Camera(int width, int height);

    void SetPosition(D3DXVECTOR2 newPos) { position = newPos; }
    void Move(D3DXVECTOR2 delta) { position += delta; }
    D3DXVECTOR2 GetPosition() const { return position; }

    // Recalculates the matrix every frame
    void Update();
    D3DXMATRIX GetViewMatrix() const { return viewMatrix; }

    // Critical for mouse input (clicks on the screen vs world position)
    D3DXVECTOR2 ScreenToWorld(D3DXVECTOR2 screenPos);
};