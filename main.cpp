#define WIN32_LEAN_AND_MEAN
#define PI 3.14159
#include <Windows.h>
#include <iostream>
#include <string>
#include <d3d9.h>
#include <d3dx9.h>
#include <cmath>
#include <dinput.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;

// Window handle
HWND g_hWnd = NULL;

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	cout << "Game Running...";

}