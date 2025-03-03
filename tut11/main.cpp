#include "glad.h"
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "filesystem.h"
#include "model.hpp"
#include "shader.hpp"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

// #define MAX_ANGLE 180.0f
// #define MIN_ANGLE 15.0f
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// forward decls.
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLFWwindow *InitWindow();

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

static float deltaTime = 0.0f;    // Time between current frame and last frame
static float lastFrame = 0.0f;    // Time of last frame
static float currentFrame = 0.0f; // Time of last frame

ShaderCode modelCode("assets/shaders/gls330/model.vert",
                     "assets/shaders/gls330/model.frag");

int main(int argc, char **argv) {

  if (!modelCode.Load()) {
    return -1;
  }

  GLFWwindow *window = InitWindow();
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    return 1;
  }

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return -1;
  }

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  Shader curShader(&modelCode);
  if (!curShader.IsValid()) {
    glfwTerminate();
    return -1;
  }
  Model curModel(
      FileSystem::getPath("resources/objects/backpack/backpack.obj"));

  while (!glfwWindowShouldClose(window)) {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    curShader.use();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    curShader.setMat4("projection", projection);
    curShader.setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    // translate it down so it's at the center of the scene
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    // it's a bit too big for our scene, so scale it down
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    curShader.setMat4("model", model);
    curModel.Draw(curShader);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime * 5);
  if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime * 5);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

GLFWwindow *InitWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    return window;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);
  // tell GLFW to capture our mouse
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  return window;
}

bool leftButtonDown = false;

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  leftButtonDown = (button == GLFW_MOUSE_BUTTON_LEFT && action == 1);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;
  if (leftButtonDown)
    camera.ProcessMouseMovement(-xoffset, -yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
