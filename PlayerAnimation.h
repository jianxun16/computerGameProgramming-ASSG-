// author by limzhenxing

#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "PlayerCollision.h"

class AudioManager;
class InputManager;
class TileMap;

// The playable warrior, animated from a single combined sheet (Warrior_Anim.png, 1024x1024, cut into 192x192 cells on a 5-column grid):
//
//        c0    c1    c2    c3      c4
//   r0  idle  idle  idle  attack  attack
//   r1  idle  idle  idle  attack  attack
//   r2  idle  idle   -      -       -
//   r3  run   run   run     -       -
//   r4  run   run   run     -       -
//
// Each animation is a rectangular BLOCK of cells (start col/row + width + count),
// read left->right, top->bottom inside the block.
//
// States (as requested):
//   1 IDLE        - not moving
//   2 RUN_LEFT    - moving left  (run frames drawn flipped, scale x = -1)
//   3 RUN_RIGHT   - moving right (run frames drawn normally)
//   4 ATTACK      - left mouse click; one-shot, then back to idle/run.
//                   While attacking a circular collision (attack range) sits
//                   in front of the character.
class PlayerAnimation
{
public:
    static const int FRAME_SIZE    = 192;
    static const int FEET_Y        = 137;  // feet position inside a cell
    static const int FEET_CENTER_X = 100;  // horizontal centre inside a cell

    enum State
    {
        STATE_IDLE      = 1,
        STATE_RUN_LEFT  = 2,
        STATE_RUN_RIGHT = 3,
        STATE_ATTACK    = 4
    };

    PlayerAnimation();
    ~PlayerAnimation();

    bool  load(IDirect3DDevice9* device, int screenWidth, int screenHeight);
    float update(InputManager* input, AudioManager* audioManager, TileMap* map);
    void  render(LPD3DXSPRITE spriteBrush);
    void  release();

    // How far the world has scrolled (used to draw the tile map / background).
    float getCameraX() const { return playerWorldPosition.x; }

    // Feet position in world space (used to spawn the player on the ground).
    float getFeetWorldX() const { return spritePosition.x + FEET_CENTER_X + playerWorldPosition.x; }

    // Feet height on screen (used to tell when the player has fallen off the map).
    float getFeetY() const { return spritePosition.y + FEET_Y; }

    // Put the character's feet on a given screen/world Y (used at spawn).
    void  setStartFeet(float feetY) { spritePosition.y = feetY - FEET_Y; velocity.y = 0; isJumping = false; }

    // Reset scroll / velocity / size back to the start (used when restarting).
    void  respawnToStart();

    // The player's collision box in world space (for item pickups etc.).
    void  getWorldHitbox(float& left, float& top, float& right, float& bottom) const;

    // Uniform zoom for the whole character. Everything (sprite + attack range)
    // scales by this around the feet, so the feet stay planted and the attack
    // circle stays proportional. Change this one value to resize safely.
    void  setScale(float s) { scale = s; }
    float getScale() const  { return scale; }

    // Attack range: a circle in front of the character, active only while
    // attacking. Returns true and fills centre (screen space) + radius when
    // active, so game code can test hits against enemies.
    bool  getAttackCircle(D3DXVECTOR3& centre, float& radius) const;

private:
    // One animation = a block of cells inside the sheet.
    struct Anim { int startCol; int startRow; int cols; int count; };

    const Anim& currentAnim() const;   // block for the current state
    int         animId() const;        // 0=idle 1=run 2=attack (frame-reset key)
    bool        facingLeft() const;    // draw mirrored?

    LPDIRECT3DTEXTURE9 sheet;          // the combined animation sheet
    LPDIRECT3DTEXTURE9 rangeTexture;   // procedural ring, drawn as attack range

    Anim idleAnim;
    Anim runAnim;
    Anim attackAnim;

    // Position / physics.
    D3DXVECTOR3 spritePosition;        // on-screen (only y changes)
    D3DXVECTOR3 playerWorldPosition;   // scrolled world position
    D3DXVECTOR3 velocity;
    float       groundY;
    int         spriteMovementSpeed;

    // Physics / tile collision, kept in its own class (see PlayerCollision.h).
    PlayerCollision collision;

    // Animation / state.
    State state;
    State lastLoggedState;   // last movement state written to the CLI log
    int   currentFrame;
    int   prevAnimId;
    float animationTimer;
    float animationSpeed;
    bool  attacking;         // an attack is playing (one-shot)
    bool  lastFacingLeft;    // remembered facing for idle / attack
    bool  isJumping;

    // Uniform character zoom (1.0 = original 192-cell size).
    float scale;

    // Attack range circle, in UNSCALED units (multiplied by `scale` at use).
    float attackRangeRadius; // radius is for the circle attack range
    float attackRangeOffset; // distance in front of the character
    float attackRangeUp;     // how far above the feet the circle sits
};
