#define WIN32_LEAN_AND_MEAN
#include <Windows.h>	
#include <iostream>
#include <fstream>
#include <d3d9.h>		
#include <d3dx9.h>
#include <dinput.h>


// Windows Interpretation of C++ for Windows only
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;


//-----GLOBAL VARIABLES-----//
HWND g_hWnd = NULL;
WNDCLASS wndClass;
IDirect3DDevice9* d3dDevice;
D3DPRESENT_PARAMETERS d3dPP;
LPD3DXFONT gameFont = NULL;
LPD3DXFONT bigFont = NULL;


/// Message
MSG msg;

RECT button1Rect;
RECT button2Rect;
RECT button3Rect;

// --- Pause menu state ---
bool isPaused = false;
RECT retryRect;
RECT exitRect;

int mouseX = 0;
int mouseY = 0;

HRESULT hr;

/// Sprite Interface
LPD3DXSPRITE spriteBrush = NULL;


LPDIRECT3DTEXTURE9 menu = NULL;
float menuWidth = 800.0f;
float menuHeight = 700.0f;

/// pointer
LPDIRECT3DTEXTURE9 button1 = NULL;
float targetWidth = 200.0f;
float targetHeight = 100.0f;

// --- Pause menu textures ---
LPDIRECT3DTEXTURE9 retryButton = NULL;
LPDIRECT3DTEXTURE9 exitButton = NULL;


/// Frame Timer stuff
//FrameTimer* gameTimer = new FrameTimer();
int gameFPS = 60;

/// Direct Input Stuff
LPDIRECTINPUT8 dInput;										//	Direct Input object.
LPDIRECTINPUTDEVICE8  dInputKeyboardDevice;					//	Pointer to Direct Input keyboard device.
BYTE  diKeys[256];											//	Key input buffer
BYTE  prevKeys[256];										//	Key input buffer from the previous frame (used to detect a fresh key press)

//--------------------------------------------------------------------

//	Window Procedure, for event handling
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);

		if (isPaused)
		{
			// While paused, only the pause menu's own buttons can be clicked.
			if (PtInRect(&retryRect, POINT{ mouseX, mouseY }))
			{
				cout << "Retry" << endl;
			}
			else if (PtInRect(&exitRect, POINT{ mouseX, mouseY }))
			{
				cout << "Exit game" << endl;
			}
			else {
				cout << "click" << endl;
			}
		}
		else
		{
			if (PtInRect(&button1Rect, POINT{ mouseX, mouseY }))
			{
				cout << "Start game" << endl;
			}
			else if (PtInRect(&button2Rect, POINT{ mouseX, mouseY }))
			{
				cout << "Setting" << endl;
			}
			else if (PtInRect(&button3Rect, POINT{ mouseX, mouseY }))
			{
				cout << "Quit game" << endl;
			}
			else {
				cout << "click" << endl;
			}
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:	// case 'Escape':
			// Quit the program when Esc is pressed
			PostQuitMessage(0);
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//--------------------------------------------------------------------

void CreateMyWindow() {
	/*
		Step 1
		Define and Register a Window.
	*/

	//	Set all members in wndClass to 0.
	ZeroMemory(&button1Rect, sizeof(button1Rect));
	ZeroMemory(&button2Rect, sizeof(button2Rect));
	ZeroMemory(&button3Rect, sizeof(button3Rect));
	ZeroMemory(&retryRect, sizeof(retryRect));
	ZeroMemory(&exitRect, sizeof(exitRect));


	//	Filling wndClass. You are to refer to MSDN for each of the members details.
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hInstance = GetModuleHandle(NULL); // Get the hInstance ID number
	wndClass.lpfnWndProc = WindowProcedure;
	wndClass.lpszClassName = "My Window";
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	//	Register the window.
	RegisterClass(&wndClass);

	/*
		Step 2
		Create the Window.
	*/
	g_hWnd = CreateWindowEx(0, wndClass.lpszClassName, "Main menu", WS_OVERLAPPEDWINDOW, 0, 100, 850, 750, NULL, NULL, GetModuleHandle(NULL), NULL);
	ShowWindow(g_hWnd, 1);

	ZeroMemory(&msg, sizeof(msg));
}

bool CreateDirectX() {
	//	Define Direct3D 9 OBJECT.
	IDirect3D9* direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);

	//	Define how the screen presents.
	ZeroMemory(&d3dPP, sizeof(d3dPP));

	d3dPP.Windowed = true;
	d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dPP.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dPP.BackBufferCount = 1;
	d3dPP.BackBufferWidth = 850;
	d3dPP.BackBufferHeight = 750;
	d3dPP.hDeviceWindow = g_hWnd;

	//	Create a Direct3D 9 device.
	hr = direct3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dPP, &d3dDevice);

	if (FAILED(hr))
		return false;

	return true;
}

void CreateDirectInput() {
	//	Create the Direct Input object.
	HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), 0x0800, IID_IDirectInput8, (void**)&dInput, NULL);

	//	Create the keyboard device.
	hr = dInput->CreateDevice(GUID_SysKeyboard, &dInputKeyboardDevice, NULL);

	//	Set the input data format.
	dInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);

	//	Set the cooperative level.
	dInputKeyboardDevice->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
}

void CleanUpDirectInput() {
	//	Release keyboard device.
	dInputKeyboardDevice->Unacquire();
	dInputKeyboardDevice->Release();
	dInputKeyboardDevice = NULL;

	//	Release DirectInput.
	dInput->Release();
	dInput = NULL;
}

void InitializeGame() {
	ZeroMemory(&button1Rect, sizeof(button1Rect));
	ZeroMemory(&diKeys, sizeof(diKeys));
	ZeroMemory(&prevKeys, sizeof(prevKeys));

	HRESULT hrSprite = D3DXCreateSprite(d3dDevice, &spriteBrush);
	HRESULT hrFont = D3DXCreateFont(d3dDevice, 24, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &gameFont);
	HRESULT hrBigFont = D3DXCreateFont(d3dDevice, 160, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &bigFont);


	if (FAILED(hrSprite)) {
		cout << "Failed to create sprite Interface / brush" << endl;
		return;
	}
	if (FAILED(hrFont)) {
		cout << "Failed to create font" << endl;
	}
	if (FAILED(hrBigFont)) {
		cout << "Failed to create big font" << endl;
	}

	// Check whether the files exist
	ifstream file("Assets/Button1.png");
	ifstream file1("Assets/menu.png");
	ifstream file2("Assets/Retry.png");
	ifstream file3("Assets/Exit.png");

	if (!file1.good()) {
		cout << "ERROR: Cannot find Assets/menu.png" << endl;
	}
	else {
		cout << "SUCCESS: Found Assets/menu.png" << endl;
	}

	if (!file.good()) {
		cout << "ERROR: Cannot find Assets/Button1.png" << endl;
	}
	else {
		cout << "SUCCESS: Found Assets/Button1.png" << endl;
	}

	if (!file2.good()) {
		cout << "ERROR: Cannot find Assets/Retry.png" << endl;
	}
	else {
		cout << "SUCCESS: Found Assets/Retry.png" << endl;
	}

	if (!file3.good()) {
		cout << "ERROR: Cannot find Assets/Exit.png" << endl;
	}
	else {
		cout << "SUCCESS: Found Assets/Exit.png" << endl;
	}

	file.close();
	file1.close();
	file2.close();
	file3.close();

	// Load textures - each gets its own HRESULT so a failure on one doesn't get masked by another
	HRESULT hrButton1 = D3DXCreateTextureFromFile(d3dDevice, "Assets/Button1.png", &button1);
	HRESULT hrMenu = D3DXCreateTextureFromFile(d3dDevice, "Assets/menu.png", &menu);
	HRESULT hrRetry = D3DXCreateTextureFromFile(d3dDevice, "Assets/Retry.png", &retryButton);
	HRESULT hrExit = D3DXCreateTextureFromFile(d3dDevice, "Assets/Exit.png", &exitButton);

	if (FAILED(hrButton1)) {
		cout << "Failed to load button1 texture. HRESULT = 0x" << hex << hrButton1 << endl;
	}
	else {
		cout << "button texture loaded successfully!" << endl;
	}

	if (FAILED(hrMenu)) {
		cout << "Failed to load menu texture. HRESULT = 0x" << hex << hrMenu << endl;
	}
	else {
		cout << "menu texture loaded successfully!" << endl;
	}

	if (FAILED(hrRetry)) {
		cout << "Failed to load retry texture. HRESULT = 0x" << hex << hrRetry << endl;
	}
	else {
		cout << "retry texture loaded successfully!" << endl;
	}

	if (FAILED(hrExit)) {
		cout << "Failed to load exit texture. HRESULT = 0x" << hex << hrExit << endl;
	}
	else {
		cout << "exit texture loaded successfully!" << endl;
	}
}

bool WindowIsRunning() {
	/*
		Step 3
		Handling window messages
	*/
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		//	Receive a quit message
		if (msg.message == WM_QUIT) {
			return false;
			break;
		}

		//	Translate the message
		TranslateMessage(&msg);
		//	Send message to your window procedure
		DispatchMessage(&msg);
	}

	return true;
}

void GetInput() {
	//	Acquire the device.
	dInputKeyboardDevice->Acquire();

	//	Get immediate Keyboard Data.
	dInputKeyboardDevice->GetDeviceState(256, diKeys);
}

void Update() {
	// Toggle the pause menu when P is freshly pressed this frame (not held from last frame).
	if ((diKeys[DIK_P] & 0x80) && !(prevKeys[DIK_P] & 0x80))
	{
		isPaused = !isPaused;
	}

	// Remember this frame's key state for next frame's edge-detection.
	memcpy(prevKeys, diKeys, sizeof(diKeys));
}

// --- Shared button-drawing helper -------------------------------------------------
// Scales "tex" to (w x h), positions it at "pos", writes the resulting screen-space
// rect into "outRect" (for click collision), and draws "label" centered on top of it.
// Using one function for every button means the position/scale/rect math only has to
// be correct in one place, instead of being copy-pasted (and drifting) per button.
void DrawButton(LPDIRECT3DTEXTURE9 tex, const D3DXVECTOR2& pos, float w, float h, RECT& outRect, const char* label)
{
	if (tex == NULL) return;

	D3DSURFACE_DESC desc;
	tex->GetLevelDesc(0, &desc);

	float scaleX = w / (float)desc.Width;
	float scaleY = h / (float)desc.Height;

	D3DXVECTOR2 scaling(scaleX, scaleY);
	D3DXVECTOR2 rotationCenter(0.0f, 0.0f);

	outRect.left = (LONG)pos.x;
	outRect.top = (LONG)pos.y;
	outRect.right = (LONG)(pos.x + w);
	outRect.bottom = (LONG)(pos.y + h);

	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &scaling, &rotationCenter, 0.0f, &pos);
	spriteBrush->SetTransform(&mat);
	spriteBrush->Draw(tex, NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

	spriteBrush->SetTransform(NULL);
	bigFont->DrawTextA(spriteBrush, label, -1, &outRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255));
	spriteBrush->SetTransform(NULL);
}

void DrawBackground(float clientWidth, float clientHeight)
{
	if (menu == NULL) return;

	D3DSURFACE_DESC menuDesc;
	menu->GetLevelDesc(0, &menuDesc);

	float menuScaleX = clientWidth / (float)menuDesc.Width;
	float menuScaleY = clientHeight / (float)menuDesc.Height;

	D3DXVECTOR2 menuScaling(menuScaleX, menuScaleY);
	D3DXVECTOR2 menuRotationCenter(0.0f, 0.0f);
	D3DXVECTOR2 menuPosition(0.0f, 0.0f);

	D3DXMATRIX menuMat;
	D3DXMatrixTransformation2D(&menuMat, NULL, 0.0f, &menuScaling, &menuRotationCenter, 0.0f, &menuPosition);
	spriteBrush->SetTransform(&menuMat);
	spriteBrush->Draw(menu, NULL, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));
}

void RenderMainMenu(float clientWidth, float clientHeight)
{
	if (button1 == NULL) return;

	D3DXVECTOR2 position(
		(clientWidth - targetWidth) * 0.5f,
		(clientHeight - targetHeight) * 0.5f
	);
	DrawButton(button1, position, targetWidth, targetHeight, button1Rect, "Start");

	float buttonGap = 20.0f;
	D3DXVECTOR2 position2(position.x, position.y + targetHeight + buttonGap);
	DrawButton(button1, position2, targetWidth, targetHeight, button2Rect, "Setting");

	float buttonGap1 = 20.0f;
	D3DXVECTOR2 position3(position2.x, position2.y + targetHeight + buttonGap1);
	DrawButton(button1, position3, targetWidth, targetHeight, button3Rect, "Quit");
}

void RenderPauseMenu(float clientWidth, float clientHeight)
{
	// Retry and Exit sit side by side, anchored near the bottom of the window.
	float bottomMargin = 40.0f;
	float gapBetween = 20.0f;
	float totalWidth = targetWidth * 2.0f + gapBetween;

	float startX = (clientWidth - totalWidth) * 0.5f;
	float bottomY = clientHeight - targetHeight - bottomMargin;

	D3DXVECTOR2 retryPos(startX, bottomY);
	D3DXVECTOR2 exitPos(startX + targetWidth + gapBetween, bottomY);

	DrawButton(retryButton, retryPos, targetWidth, targetHeight, retryRect, "Retry");
	DrawButton(exitButton, exitPos, targetWidth, targetHeight, exitRect, "Exit");
}

void Render() {
	//	Clear the back buffer.
	d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	//	Begin the scene - unlock the back buffer
	d3dDevice->BeginScene();

	//	Specify alpha blend will ensure that the sprite will render with alpha.
	spriteBrush->Begin(D3DXSPRITE_ALPHABLEND);

	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);
	float clientWidth = (float)(clientRect.right - clientRect.left);
	float clientHeight = (float)(clientRect.bottom - clientRect.top);

	DrawBackground(clientWidth, clientHeight);

	if (isPaused)
	{
		RenderPauseMenu(clientWidth, clientHeight);
	}
	else
	{
		RenderMainMenu(clientWidth, clientHeight);
	}

	spriteBrush->End();                // 
	d3dDevice->EndScene();             // 
	d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void CleanUpSprite() {
	spriteBrush->Release();
	spriteBrush = NULL;

	if (gameFont != NULL) {
		gameFont->Release();
		gameFont = NULL;
	}
	if (bigFont != NULL) {
		bigFont->Release();
		bigFont = NULL;
	}

	if (button1 != NULL) {
		button1->Release();
		button1 = NULL;
	}
	if (retryButton != NULL) {
		retryButton->Release();
		retryButton = NULL;
	}
	if (exitButton != NULL) {
		exitButton->Release();
		exitButton = NULL;
	}
}

void CleanUpDirectX() {
	//	Release the device when exiting.
	d3dDevice->Release();
	//	Reset pointer to NULL, a good practice.
	d3dDevice = NULL;
}

void CleanUpWindow() {
	//	Free up the memory.
	UnregisterClass(wndClass.lpszClassName, GetModuleHandle(NULL));
}

//--------------------------------------------------------------------

//	use WinMain if you don't want the console
int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	CreateMyWindow();
	if (!CreateDirectX())
		return 0;

	CreateDirectInput();
	InitializeGame();

	while (WindowIsRunning())
	{
		GetInput();
		Update();
		Render();
	}

	CleanUpSprite();
	CleanUpDirectInput();
	CleanUpDirectX();
	CleanUpWindow();

	return 0;
}
//--------------------------------------------------------------------

