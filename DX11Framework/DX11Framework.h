#pragma once

#include <windows.h>
#include "SceneManager.h"
#include "Renderer.h"

class DX11Framework
{
public:
	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	~DX11Framework();
	void Update();
	void Render();

private:
	HRESULT CreateWindowHandle(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitRenderer();

private:
	HWND m_WindowHandle;
	ScreenInformation m_ScreenInfo;
	SceneManager* m_SceneManager = nullptr;
	RenderingSystem* m_RenderSystem = nullptr;
};