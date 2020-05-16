#include "src/imgui/imgui.h"
#include "src/platform/glfw/glfw.h"
#include "src/renderer/opengl3/opengl3.h"
#include <stdio.h>
#include <windows.h>
#include <winuser.h>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load
//  OpenGL function pointers. Helper libraries are often used for this purpose!
//  Here we are supporting a few common ones (gl3w, glew, glad). We use gl3w.
#include "src/gl3w/gl3w.h" // Initialize with gl3wInit()

// Include glfw3.h after our OpenGL definitions
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

//#define WINVER 0x0500
#include <windows.h>

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void ReleaseFocus(GLFWwindow *window, int focused) {
  if (focused) {
    auto hwnd = glfwGetWin32Window(window);
    // Does nott work
    auto foregroundHwnd = GetForegroundWindow();
    auto nextHwnd = GetNextWindow(foregroundHwnd, GW_HWNDNEXT);

    SetForegroundWindow(nextHwnd);
  }
}

void ReleaseFocusOnMouseClick(GLFWwindow *window, int button, int action,
                              int mods) {
  auto hwnd = glfwGetWin32Window(window);
  // Does nott work
  auto foregroundHwnd = GetForegroundWindow();
  auto nextHwnd = GetNextWindow(foregroundHwnd, GW_HWNDNEXT);

  SetForegroundWindow(nextHwnd);
}

int main(int, char **) {
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only

  // Create window with graphics context
  GLFWwindow *window = glfwCreateWindow(1280, 720, "ButtonBox", NULL, NULL);
  if (window == NULL)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // glfwSetWindowFocusCallback(window, ReleaseFocus);
  // glfwSetMouseButtonCallback(window, ReleaseFocusOnMouseClick);
  // Initialize OpenGL loader
  bool err = gl3wInit() != 0;
  if (err) {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return 1;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0);

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
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(io.DisplaySize);
      auto open = true;

      ImGui::Begin("ButtonBoxWindow", &open, ImGuiWindowFlags_NoDecoration);

      ImGui::Text("This is some useful text.");

      auto cp = ImGui::GetCursorPos();
      if (ImGui::Button("A", ImVec2(600, 600))) {
        buttonA.ki.dwFlags = 0;
        SendInput(1, &buttonA, sizeof(INPUT));
        buttonA.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &buttonA, sizeof(INPUT));
      }
      ImGui::SetCursorPos(ImVec2(620, cp.y));

      if (ImGui::Button("B", ImVec2(600, 600))) {
        // auto hwnd = glfwGetWin32Window(window);
        // auto nextHwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
        // SetForegroundWindow(nextHwnd);

        buttonB.ki.dwFlags = 0;
        SendInput(1, &buttonB, sizeof(INPUT));
        buttonB.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &buttonB, sizeof(INPUT));
      }

      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
