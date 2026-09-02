#include "AnimationController.h"

void AnimationController::Initialize(int texWidth, int texHeight, int cols, int rows, int frames, float speed, CycleDirection dir) {
    maxCols = cols;
    maxRows = rows;

    frameWidth = texWidth / maxCols;
    frameHeight = texHeight / maxRows;

    totalFrames = frames;
    currentFrame = 0;
    activeTrack = 0;

    timePerFrame = speed;
    timer = 0.0f;

    direction = dir;
}

void AnimationController::Update(float deltaTime) {
    timer += deltaTime;

    if (timer >= timePerFrame) {
        timer -= timePerFrame;
        currentFrame++;

        if (currentFrame >= totalFrames) {
            currentFrame = 0;
        }
    }
}

void AnimationController::SetTrack(int trackIndex) {
    activeTrack = trackIndex;
}

void AnimationController::SetDirection(CycleDirection dir) {
    direction = dir;
}

RECT AnimationController::GetSourceRect() const {
    int col = 0;
    int row = 0;

    switch (direction) {
    case CycleDirection::LeftToRight:
        col = currentFrame;
        row = activeTrack;
        break;

    case CycleDirection::RightToLeft:
        col = (maxCols - 1) - currentFrame;
        row = activeTrack;
        break;

    case CycleDirection::TopToBottom:
        col = activeTrack;
        row = currentFrame;
        break;

    case CycleDirection::BottomToTop:
        col = activeTrack;
        row = (maxRows - 1) - currentFrame;
        break;
    }

    // convert grid to bounds
    RECT rect;
    rect.left = col * frameWidth;
    rect.top = row * frameHeight;
    rect.right = rect.left + frameWidth;
    rect.bottom = rect.top + frameHeight;

    return rect;
}