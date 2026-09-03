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

// One animation described as a rectangular BLOCK of cells on the sheet: it
// starts at (startCol, startRow) and reads `count` frames left->right,
// top->bottom inside a block that is `cols` cells wide. This matches sheets
// where a single animation spans several rows (e.g. the warrior's 8-frame idle
// laid out over rows 0-2), which the simple "one track = one row" model cannot.
struct AnimClip {
    int startCol;
    int startRow;
    int cols;
    int count;
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

    // ---- Block/clip mode (used by the player) ----
    AnimClip clip;   // the animation currently playing
    bool     useClip;// true = read cells from `clip`; false = grid/track mode

public:
    // Grid/track mode: one row = one track, one column = one frame.
    void Initialize(int texWidth, int texHeight, int cols, int rows, int frames, float speed, CycleDirection dir);

    // Block/clip mode: describe the sheet grid once, then play named blocks.
    void SetupSheet(int texWidth, int texHeight, int gridCols, int gridRows, float speed);
    void PlayClip(const AnimClip& c);   // switch clip; restarts only if it changed

    void Update(float deltaTime);

    void Reset();   // snap back to the first frame (call when the track changes)

    void SetTrack(int trackIndex);
    void SetDirection(CycleDirection dir);

    RECT GetSourceRect() const;
};