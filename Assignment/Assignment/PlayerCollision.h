// author by limzhenxing

#pragma once
#include <d3d9.h>
#include <d3dx9.h>

class TileMap;

// Handles the player's physics against the tile map, split out of
// PlayerAnimation so movement/collision lives in its own class (OOP):
//   - horizontal blocking (don't scroll the world into a solid tile),
//   - gravity + landing on the top of a tile,
//   - bonking the head on a tile above.
//
// The player still OWNS its position / velocity; this class only reads and
// resolves them. The hitbox size lives here as the single source of truth so
// both movement and item-pickup use the exact same box.
class PlayerCollision
{
public:
    PlayerCollision();

    // Move the player by `velocity` this frame and resolve tile collisions.
    // feetCenterX / feetY are the feet-anchor offsets inside the sprite cell
    // (PlayerAnimation::FEET_CENTER_X / FEET_Y). spritePos.y, worldPos.x,
    // velocity and isJumping are all updated in place. When map is NULL it
    // falls back to a flat ground line at groundY.
    void resolve(TileMap* map,
                 float feetCenterX, float feetY,
                 float scale,
                 D3DXVECTOR3& spritePos,
                 D3DXVECTOR3& worldPos,
                 D3DXVECTOR3& velocity,
                 bool& isJumping,
                 float groundY);

    // Downward pull applied every frame.
    void  setGravity(float g) { gravity = g; }
    float getGravity() const  { return gravity; }

    // Hitbox dimensions in SCALED world units (unscaled value * scale). Kept
    // here so PlayerAnimation::getWorldHitbox reuses the same numbers.
    float getHalfWidth(float scale) const  { return hitHalfWidth * scale; }
    float getBodyHeight(float scale) const { return bodyHeight   * scale; }

private:
    float gravity;
    float hitHalfWidth;   // unscaled half width of the body box
    float bodyHeight;     // unscaled height (feet up to head)
};
