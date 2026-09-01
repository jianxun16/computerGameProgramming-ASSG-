// author by limzhenxing

#pragma once
#include <d3d9.h>
#include <d3dx9.h>

class TileMap;

// Player physics against the tile map, split out of PlayerAnimation (OOP):
// horizontal blocking, gravity + landing, and head-bonk. The player owns its
// position/velocity; this only resolves them. The hitbox size lives here as the
// single source of truth for movement and item pickup.
class PlayerCollision
{
public:
    PlayerCollision();

    // Move by `velocity` and resolve tile collisions. feetCenterX / feetY are
    // the feet-anchor offsets in the sprite cell; spritePos.y, worldPos.x,
    // velocity and isJumping update in place. NULL map = flat ground at groundY.
    void resolve(TileMap* map,
                 float feetCenterX, float feetY,
                 float scale,
                 D3DXVECTOR3& spritePos,
                 D3DXVECTOR3& worldPos,
                 D3DXVECTOR3& velocity,
                 bool& isJumping,
                 float groundY);

    // Downward pull per frame.
    void  setGravity(float g) { gravity = g; }
    float getGravity() const  { return gravity; }

    // Hitbox dimensions in scaled world units (reused by getWorldHitbox).
    float getHalfWidth(float scale) const  { return hitHalfWidth * scale; }
    float getBodyHeight(float scale) const { return bodyHeight   * scale; }

private:
    float gravity;
    float hitHalfWidth;   // unscaled half width
    float bodyHeight;     // unscaled height
};
