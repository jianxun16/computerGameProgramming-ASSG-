// author by limzhenxing

#include "PlayerCollision.h"
#include "TileMap.h"
#include <math.h>

PlayerCollision::PlayerCollision()
{
    gravity      = 2.0f;
    hitHalfWidth = 8.0f;
    bodyHeight   = 15.0f;
}

void PlayerCollision::resolve(TileMap* map,
                              float feetCenterX, float feetYOffset,
                              float scale,
                              D3DXVECTOR3& spritePos,
                              D3DXVECTOR3& worldPos,
                              D3DXVECTOR3& velocity,
                              bool& isJumping,
                              float groundY)
{
    if (map != NULL)
    {
        float feetScreenX = spritePos.x + feetCenterX;  // fixed on screen
        float halfW = hitHalfWidth * scale;             // hitbox half width
        float bodyH = bodyHeight   * scale;             // hitbox height

        // Horizontal: scroll the world, but block if the body would enter a solid tile.
        float tryCamX = worldPos.x + velocity.x;
        float feetWX  = feetScreenX + tryCamX;
        float feetY   = spritePos.y + feetYOffset;
        if (map->rectSolid(feetWX - halfW, feetY - bodyH, feetWX + halfW, feetY - 4.0f))
            velocity.x = 0;
        worldPos.x += velocity.x;

        // Vertical: gravity pulls, then land on or bonk a tile.
        velocity.y += gravity;
        spritePos.y += velocity.y;

        float camX = worldPos.x;
        feetWX = feetScreenX + camX;
        feetY  = spritePos.y + feetYOffset;

        if (velocity.y > 0.0f)   // falling -> stand on a tile top
        {
            if (map->rectSolid(feetWX - halfW, feetY - bodyH, feetWX + halfW, feetY))
            {
                int row = (int)floorf(feetY / TileMap::TILE);
                spritePos.y = row * (float)TileMap::TILE - feetYOffset;
                velocity.y = 0;
                isJumping = false;
            }
            else
            {
                isJumping = true;   // walked off a ledge
            }
        }
        else if (velocity.y < 0.0f)   // rising -> bonk head
        {
            if (map->rectSolid(feetWX - halfW, feetY - bodyH, feetWX + halfW, feetY))
            {
                int row = (int)floorf((feetY - bodyH) / TileMap::TILE);
                spritePos.y = (row + 1) * (float)TileMap::TILE + bodyH - feetYOffset;
                velocity.y = 0;
            }
        }
    }
    else
    {
        // No tile map -> flat ground fallback.
        worldPos.x += velocity.x;
        spritePos.y += velocity.y;
        if (spritePos.y >= groundY)
        {
            spritePos.y = groundY;
            velocity.y = 0;
            isJumping = false;
        }
    }
}
