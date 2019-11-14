// silence opengl deprecation warnings for osx
#define GL_SILENCE_DEPRECATION

// avoid including both gl.h and gl3.h
#define GLFW_INCLUDE_GLCOREARB

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#include "./shader.hpp"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(int w, int h);

// screen settings
static unsigned int width = 800, height = 600;

int main(int argc, char* argv[])
{
  GLFWwindow* window = initWindow(width, height);

  if (!window) {
    return -1;
  }

  // bind window callbacks
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSwapInterval(1);

  // build and compile shaders
  // -------------------------
  Shader shader("the-hello-world-triangle/shader.vs", "the-hello-world-triangle/shader.fs");

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] = { 0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f };
  GLuint vbo, vao;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  while (!glfwWindowShouldClose(window)) {
    // render
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // cleanups
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

GLFWwindow* initWindow(int w, int h)
{
  if (!glfwInit())
    return NULL;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  // osx specific
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create window
  GLFWwindow* window = glfwCreateWindow(width, height, "Hello World Triangle", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window);
  return window;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
