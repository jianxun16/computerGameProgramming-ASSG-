#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <map>

using namespace std;

class AssetManager {
private:
    map<string, LPDIRECT3DTEXTURE9> textures;

public:
    LPDIRECT3DTEXTURE9 GetTexture(IDirect3DDevice9* device, string path);
    void CleanUpAssets();
};