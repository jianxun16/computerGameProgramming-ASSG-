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

    useClip = false;   // grid/track mode
}

void AnimationController::SetupSheet(int texWidth, int texHeight, int gridCols, int gridRows, float speed) {
    maxCols = gridCols;
    maxRows = gridRows;

    frameWidth = texWidth / maxCols;
    frameHeight = texHeight / maxRows;

    timePerFrame = speed;
    timer = 0.0f;
    currentFrame = 0;

    useClip = true;
    clip = AnimClip{ -1, -1, -1, -1 };   // force the first PlayClip to start fresh
}

void AnimationController::PlayClip(const AnimClip& c) {
    // Restart only when the animation actually changes, so holding a key (same
    // clip every frame) does not stutter on frame 0.
    if (clip.startCol != c.startCol || clip.startRow != c.startRow ||
        clip.cols != c.cols || clip.count != c.count) {
        clip = c;
        currentFrame = 0;
        timer = 0.0f;
    }
}

void AnimationController::Update(float deltaTime) {
    timer += deltaTime;

    if (timer >= timePerFrame) {
        timer -= timePerFrame;
        currentFrame++;

        int frames = useClip ? clip.count : totalFrames;
        if (frames <= 0) frames = 1;
        if (currentFrame >= frames) {
            currentFrame = 0;
        }
    }
}

void AnimationController::Reset() {
    currentFrame = 0;
    timer = 0.0f;
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

    // Block/clip mode: walk the current frame across the clip's block of cells.
    if (useClip) {
        int localCol = (clip.cols > 0) ? (currentFrame % clip.cols) : 0;
        int localRow = (clip.cols > 0) ? (currentFrame / clip.cols) : 0;
        col = clip.startCol + localCol;
        row = clip.startRow + localRow;

        RECT rect;
        rect.left = col * frameWidth;
        rect.top = row * frameHeight;
        rect.right = rect.left + frameWidth;
        rect.bottom = rect.top + frameHeight;
        return rect;
    }

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