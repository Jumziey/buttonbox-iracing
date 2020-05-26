// dear imgui - standalone example application for DirectX 11
// If you are new to dear imgui, see examples/README.txt and documentation at
// the top of imgui.cpp.

#include "buttonbox.h"
#include "src/imgui/imgui.h"
#include <ctime>
#include <iomanip>
#include <iostream>

// First off: this platform and render code is
// hillariously messy. I mean its like pure pain to work with.
// Need to see what i can do about that...
//
// EnableWindow(hwnd, false) should be called
// when clicking on the window
//
// EnableWindow(hwnd, true) should be called when
// clicking on the title bar.
//
// When Window is disabled i can use
// SET_CURSOR and look at the mode of the mouse move there
// Then also register mouse pos with GetMessagePos(), remember to
// do a transformation to the window placement
//
// How do i check if the title bar is clicked? What the position and size of the
// title bar?
//
// This need to be properly implemented with the imgui/platform/win32
// code
//
// Note that we should try and sort out the platform code
// perhaps with a proper interface both win32 and glfw can fulfill
//
// But how do we then handle the special code with focus on win32?
// Maybe its not completly needed yet...
//

// Main code
int main(int, char **) {
  // Create application window
  WNDCLASSEX wc;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_CLASSDC;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0L;
  wc.cbWndExtra = 0L;
  wc.hInstance = GetModuleHandle(NULL);
  wc.hIcon = NULL;
  wc.hCursor = NULL;
  wc.hbrBackground = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = _T("Buttonboxn");
  wc.hIconSm = NULL;

  ::RegisterClassEx(&wc);
  HWND hwnd = ::CreateWindow(
      wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW,
      100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

  // Initialize Direct3D
  if (!CreateDeviceD3D(hwnd)) {
    CleanupDeviceD3D();
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 1;
  }

  // Show the window
  ::ShowWindow(hwnd, SW_SHOWDEFAULT);
  ::UpdateWindow(hwnd);

  // Disable focus
  ::EnableWindow(hwnd, false);
  // Should disable on press which isnt header
  // if header pressed should enable again
  // Better then doing it like this

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
  // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

  ImVec4 background_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  INPUT buttonA;
  // Set up a generic keyboard event.
  buttonA.type = INPUT_KEYBOARD;
  buttonA.ki.wScan = 0; // hardware scan code for key
  buttonA.ki.time = 0;
  buttonA.ki.dwExtraInfo = 0;
  buttonA.ki.wVk = 0x41; // virtual-key code for the "a" key

  INPUT buttonB;
  // Set up a generic keyboard event.
  buttonB.type = INPUT_KEYBOARD;
  buttonB.ki.wScan = 0; // hardware scan code for key
  buttonB.ki.time = 0;
  buttonB.ki.dwExtraInfo = 0;
  buttonB.ki.wVk = 0x42; // virtual-key code for the "a" key

  // Main loop
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  while (msg.message != WM_QUIT) {
    // Poll and handle messages (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application. Generally you may always pass all inputs
    // to dear imgui, and hide them from your application based on those two
    // flags.
    if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
      continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(io.DisplaySize);
      auto open = true;

      ImGui::Begin("ButtonBoxWindow", &open, ImGuiWindowFlags_NoDecoration);

      auto cp = ImGui::GetCursorPos();
      if (ImGui::Button("A", ImVec2(600, 600))) {
        std::cout << "Pressed A" << std::endl;
      }
      ImGui::SetCursorPos(ImVec2(620, cp.y));

      if (ImGui::Button("B", ImVec2(600, 600))) {
        std::cout << "Pressed B" << std::endl;
      }

      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView,
                                               (float *)&background_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(0, 0); // Present without vsync (1,0) to enable vsync
  }

  // Cleanup
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  CleanupDeviceD3D();
  ::DestroyWindow(hwnd);
  ::UnregisterClass(wc.lpszClassName, wc.hInstance);

  return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd) {
  // Setup swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT createDeviceFlags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };
  if (D3D11CreateDeviceAndSwapChain(
          NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
          featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
          &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
    return false;

  CreateRenderTarget();
  return true;
}

void CleanupDeviceD3D() {
  CleanupRenderTarget();
  if (g_pSwapChain) {
    g_pSwapChain->Release();
    g_pSwapChain = NULL;
  }
  if (g_pd3dDeviceContext) {
    g_pd3dDeviceContext->Release();
    g_pd3dDeviceContext = NULL;
  }
  if (g_pd3dDevice) {
    g_pd3dDevice->Release();
    g_pd3dDevice = NULL;
  }
}

void CreateRenderTarget() {
  ID3D11Texture2D *pBackBuffer;
  g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL,
                                       &g_mainRenderTargetView);
  pBackBuffer->Release();
}

void CleanupRenderTarget() {
  if (g_mainRenderTargetView) {
    g_mainRenderTargetView->Release();
    g_mainRenderTargetView = NULL;
  }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    return true;

  switch (msg) {
  case WM_SIZE:
    if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
      CleanupRenderTarget();
      g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam),
                                  DXGI_FORMAT_UNKNOWN, 0);
      CreateRenderTarget();
    }
    return 0;
  case WM_SETCURSOR: {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::cout << "WndProc called" << std::put_time(&tm, "%c %Z") << std::endl;
    return 0; // Can be used to avoid the bells
  }
  case WM_SYSCOMMAND:
    if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
      return 0;
    break;
  case WM_DESTROY:
    ::PostQuitMessage(0);
    return 0;
  }
  return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
