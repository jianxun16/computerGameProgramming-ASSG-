#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace std;

enum class CycleDirection { // how to read sprite
    LeftToRight,
    RightToLeft,
    TopToBottom,
    BottomToTop
};

class AnimationController {
private:
    int frameWidth;
    int frameHeight;
    int maxCols;
    int maxRows;

    int currentFrame;
    int totalFrames;
    int activeTrack; // which direction, walk left, walk right, etc

    float timePerFrame;
    float timer;

    CycleDirection direction;

public:
    void Initialize(int texWidth, int texHeight, int cols, int rows, int frames, float speed, CycleDirection dir);

    void Update(float deltaTime);

    void Reset();   // snap back to the first frame (call when the track changes)

    void SetTrack(int trackIndex);
    void SetDirection(CycleDirection dir);

    int GetCurrentFrame() const { return currentFrame; }

    RECT GetSourceRect() const;
};