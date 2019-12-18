#include "imgui/imgui.h"

#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include <stdio.h>

#include <iostream>
#include <vector>

#include <glad/glad.h> // Initialize with gladLoadGL()

// Include glfw3.h after OpenGL definitions
#include <GLFW/glfw3.h>

using namespace std;

static void glfw_error_callback(int error, const char* description) { fprintf(stderr, "Glfw Error %d: %s\n", error, description); }

GLFWwindow* initWindow(int w, int h, string title);

int main(int, char**)
{
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  // Create window with graphics context
  GLFWwindow* window = initWindow(1280, 720, "Dear ImGui GLFW+OpenGL4 example");
  glfwSwapInterval(1); // Enable vsync

  // Initialize OpenGL loader
  bool err = gladLoadGL() == 0;
  if (err) {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return 1;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char* glsl_version = "#version 410";
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  ImFont* font = io.Fonts->AddFontFromFileTTF("imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);
  IM_ASSERT(font != NULL);

  // Our state
  bool show_demo_window = true;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // FPS chart
    {
      static std::vector<float> vals;
      float rate = ImGui::GetIO().Framerate;
      vals.push_back(rate);
      if (vals.size() > 100)
        vals.erase(vals.begin());
      static bool open = true;
      ImGui::Begin("FPS", &open, ImGuiWindowFlags_NoResize);
      ImGui::PlotLines("", &vals[0], vals.size(), 0, NULL, 0, 80, ImVec2(300, 50));
      ImGui::End();
    }

    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    {
      static float f = 0.0f;
      static int counter = 0;
      ImGui::Begin("Hello, world!");
      ImGui::Text("This is some useful text.");
      ImGui::Checkbox("Demo Window", &show_demo_window);
      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
      ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
      if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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

GLFWwindow* initWindow(int w, int h, string title)
{
  if (!glfwInit())
    return NULL;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  // osx specific
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create window
  GLFWwindow* window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
  if (!window) {
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window);
  return window;
}
