#include "DX11Framework.h"
#include <string>

//#define RETURNFAIL(x) if(FAILED(x)) return x;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

HRESULT DX11Framework::Initialise(HINSTANCE hInstance, int nShowCmd)
{
    HRESULT hr = S_OK;

    hr = CreateWindowHandle(hInstance, nShowCmd);
    if (FAILED(hr)) return E_FAIL;

    hr = InitRenderer();
    if (FAILED(hr)) return E_FAIL;

    return hr;
}

HRESULT DX11Framework::CreateWindowHandle(HINSTANCE hInstance, int nCmdShow)
{
    const wchar_t* windowName  = L"DX11Framework";

    WNDCLASSW wndClass;
    wndClass.style = 0;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = 0;
    wndClass.hIcon = 0;
    wndClass.hCursor = 0;
    wndClass.hbrBackground = 0;
    wndClass.lpszMenuName = 0;
    wndClass.lpszClassName = windowName;

    RegisterClassW(&wndClass);

    m_ScreenInfo = ScreenInformation(1280, 720);

    m_WindowHandle = CreateWindowExW(0, windowName, windowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 
                                     m_ScreenInfo.WindowWidth, m_ScreenInfo.WindowHeight, nullptr, nullptr, hInstance, nullptr);

    return S_OK;
}

HRESULT DX11Framework::InitRenderer()
{
    m_RenderSystem = new RenderingSystem(m_WindowHandle, m_ScreenInfo);
    m_RenderSystem->Initialise();
    // Create a scene manager this will handle all scenes
    m_SceneManager = new SceneManager(m_RenderSystem);
    m_SceneManager->SetSelfRef(m_SceneManager); // set scene manager ref inside itself, this needs to be passed to scenes.
    m_SceneManager->ChangeScreen(SCREEN_Main); // Change the screen to the main screen for now.

    return S_OK;
}

DX11Framework::~DX11Framework()
{
    if (m_RenderSystem) m_RenderSystem->Release();

    if (m_SceneManager)
    {
        delete m_SceneManager;
        m_SceneManager = nullptr;
    }

    
}

void DX11Framework::Update()
{
    //Static initializes this value only once    
    static ULONGLONG frameStart = GetTickCount64();

    ULONGLONG frameNow = GetTickCount64();
    float deltaTime = (frameNow - frameStart) / 1000.0f;
    frameStart = frameNow;

    static float simpleCount = 0.0f;
    simpleCount += deltaTime;

    // Update our scene manager, this is critical to call all further updates on gameobjecst and components.
    m_SceneManager->Update(deltaTime);

    m_RenderSystem->Update(deltaTime);
}

void DX11Framework::Render()
{    

    // Call Render on Scenes/ Objects. this will gather all data before we draw everything to buffer/screen
    m_SceneManager->Render();
    m_SceneManager->RenderSkybox();

    // Call render and draw all objects on renderer.
    m_RenderSystem->Render();
}
