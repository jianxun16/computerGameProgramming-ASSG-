#pragma once
#include <d3dx9.h>
#include "Graphics.h"
#include "AnimationController.h"
#include "Camera.h"

using namespace std;

class GameObject {
protected:
    D3DXVECTOR2 position;
    D3DXVECTOR2 scale;
    float rotation;

    int spriteWidth; // for center calc
    int spriteHeight;

    LPDIRECT3DTEXTURE9 texture;
    AnimationController animator;
    bool isAnimated;

    bool active;

public:
    GameObject();
    virtual ~GameObject() {}

    virtual void Initialize(LPDIRECT3DTEXTURE9 tex, D3DXVECTOR2 startPos, int width, int height);
    virtual void SetupAnimation(int texWidth, int texHeight, int cols, int rows, int frames, float speed, CycleDirection dir);

    virtual void UpdateLogic(float deltaTime);
    virtual void RenderFrame(Graphics* graphics, Camera* camera);

    D3DXVECTOR2 GetPosition() const { return position; }
    void SetPosition(D3DXVECTOR2 pos) { position = pos; }

    int GetWidth() const { return spriteWidth; }
    int GetHeight() const { return spriteHeight; }

    AnimationController* GetAnimator() { return &animator; }

    bool IsActive() const { return active; }
    void SetActive(bool state) { active = state; }
};