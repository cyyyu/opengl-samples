// silence opengl deprecation warnings for osx
#define GL_SILENCE_DEPRECATION

// avoid including both gl.h and gl3.h
#define GLFW_INCLUDE_GLCOREARB

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#include "camera.hpp"
#include "model.h"
#include "shader.hpp"

using namespace std;

static void error_callback(int error, const char* desc);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xPos, double yPos);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void processInput(GLFWwindow* window);
GLFWwindow* initWindow(int w, int h);

// screen settings
static unsigned int width = 800, height = 600;

// camera
Camera camera(glm::vec3(0.0f, 4.0f, 12.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -30.0f);
float lastX = width / 2.0f, lastY = height / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f, lastFrame = 0.0f;

int main(int argc, char* argv[])
{
  GLFWwindow* window = initWindow(width, height);

  if (!window) {
    return -1;
  }

  // bind window callbacks
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSwapInterval(1);

  glEnable(GL_DEPTH_TEST);

  Model sphereModel("shadow/test_shapes_ssphere.obj");
  Shader shader("shadow/shader.vs", "shadow/shader.fs");
  Shader depthShader("shadow/depth_shader.vs", "shadow/depth_shader.fs");

  // depth map object
  // texture to use as fbo's depth buffer
  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
  unsigned int depthMapFBO;
  unsigned int depthMap;
  glGenFramebuffers(1, &depthMapFBO);
  {
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  // make a plane
  unsigned int planeVBO, planeVAO;
  {
    glGenVertexArrays(1, &planeVAO);
    float vertices[] = {
      -1.0, -1.0, -1.0, 0.0, 1.0, 0.0, // vertices + normals
      -1.0, -1.0, 1.0, 0.0, 1.0, 0.0,  //
      1.0, -1.0, 1.0, 0.0, 1.0, 0.0,   //
      -1.0, -1.0, -1.0, 0.0, 1.0, 0.0, //
      1.0, -1.0, 1.0, 0.0, 1.0, 0.0,   //
      1.0, -1.0, -1.0, 0.0, 1.0, 0.0,  //
    };
    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(planeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
  }

  glm::vec3 lightPos = glm::vec3(3.0f);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // animate light
    lightPos.x = glm::sin(currentFrame) * 6;
    lightPos.z = glm::cos(currentFrame) * 6;

    float near_plane = 1.0f, far_plane = 100.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    // gen depth map with framebuffer
    {
      depthShader.use();
      depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
      depthShader.setMat4("model", glm::mat4(1.0f));

      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);
      {
        // render the plane
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f)); // scale bigger
        depthShader.setMat4("model", model);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
      }
      {
        // render the sphere
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        depthShader.setMat4("model", model);
        sphereModel.Draw(depthShader);
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // render normal scene
    {
      // reset viewport
      glViewport(0, 0, width * 2, height * 2);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // projection matrics
      glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / height, 0.1f, 100.0f);
      glm::mat4 view = camera.GetViewMatrix();

      shader.use();
      shader.setMat4("projection", projection);
      shader.setMat4("view", view);
      shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
      shader.setVec3("cameraPos", camera.Position);
      shader.setVec3("lightPos", lightPos);

      // render the plane
      {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
        shader.setMat4("model", model);
        shader.setVec3("objColor", glm::vec3(1.0f));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
      }

      // render the sphere
      {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        shader.setMat4("model", model);
        shader.setVec3("objColor", glm::vec3(1.0f, 0.0f, 0.0f));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        sphereModel.Draw(shader);
      }
    }

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

  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  // osx specific
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create window
  GLFWwindow* window = glfwCreateWindow(width, height, "Model Loading", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window);
  return window;
}

static void processInput(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera.ProcessKeyboard(UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera.ProcessKeyboard(DOWN, deltaTime);
}

static void error_callback(int error, const char* desc) { fprintf(stderr, "GLFW Error: %s\n", desc); }

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  else if (key == GLFW_KEY_G && action == GLFW_PRESS)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else if (key == GLFW_KEY_G && action == GLFW_RELEASE)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

static void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
  if (firstMouse == true) {
    lastX = xPos;
    lastY = yPos;
    firstMouse = false;
  }

  float xoffset = xPos - lastX;
  float yoffset = yPos - lastY;

  lastX = xPos;
  lastY = yPos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) { camera.ProcessMouseScroll(yoffset); }
