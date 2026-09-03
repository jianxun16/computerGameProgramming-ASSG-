#include "AssetManager.h"
#include <iostream>

LPDIRECT3DTEXTURE9 AssetManager::GetTexture(IDirect3DDevice9* device, string path) {
    // check if texture exist and return it if true
    if (textures.find(path) != textures.end()) {
        return textures[path];
    }

    // if not exist, then load
    LPDIRECT3DTEXTURE9 newTexture = NULL;
    if (SUCCEEDED(D3DXCreateTextureFromFileEx(device, path.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &newTexture)))
    {
        textures[path] = newTexture;
        return newTexture;
    }

    cout << "Failed to load asset: " << path << endl;
    return NULL;
}

void AssetManager::CleanUpAssets() {
    // pair is the same as (as pair (i,v) in lua）
    for (auto const& pair : textures) {
        if (pair.second) {
            pair.second->Release();
        }
    }
    textures.clear();
}