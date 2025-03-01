#include "glad.h"
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "texture.hpp"

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
Camera camera(glm::vec3(0.5f, 0.2f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

static float deltaTime = 0.0f;    // Time between current frame and last frame
static float lastFrame = 0.0f;    // Time of last frame
static float currentFrame = 0.0f; // Time of last frame

// static glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
// static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// static float mixValue = 0.5f;
// static float fovValue = 45.0f;
//
ShaderCode targetCode("assets/shaders/gls330/target.vert",
                      "assets/shaders/gls330/target.frag");

ShaderCode sourceCode("assets/shaders/gls330/source.vert",
                      "assets/shaders/gls330/source.frag");

const char *textureFileName = "assets/textures/container2.png";
TextureOptions textureOptions;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main(int argc, char **argv) {

  if (!targetCode.Load() || !sourceCode.Load()) {
    return -1;
  }

  GLFWwindow *window = InitWindow();

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return -1;
  }
  // don't forget to activate the shader before setting uniforms!
  glEnable(GL_DEPTH_TEST);

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  float vertices[] = {
      // positions          // normals           // texture coords
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
  };

  Shader targetShader(targetCode.vertexCode, targetCode.fragmentCode);
  Shader sourceShader(sourceCode.vertexCode, sourceCode.fragmentCode);
  if (!targetShader.IsValid() || !sourceShader.IsValid()) {
    glfwTerminate();
    return -1;
  }

  // first, configure the target cube's VAO (and VBO)
  unsigned int VBO, targetVAO;
  glGenVertexArrays(1, &targetVAO);
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindVertexArray(targetVAO);
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  // second, configure the light's VAO (VBO stays the same; the vertices are the
  // same for the light object which is also a 3D cube)
  unsigned int sourceVAO;
  glGenVertexArrays(1, &sourceVAO);
  glBindVertexArray(sourceVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  unsigned int diffuseMap = MakeTexture(textureFileName, &textureOptions);
  targetShader.use();
  targetShader.setInt("material.diffuse", 0);

  while (!glfwWindowShouldClose(window)) {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // be sure to activate shader when setting uniforms/drawing objects
    targetShader.use();
    targetShader.setVec3("light.position", lightPos);
    targetShader.setVec3("viewPos", camera.Position);

    targetShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    targetShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    targetShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    targetShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    targetShader.setFloat("material.shininess", 64.0f);

    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    targetShader.setMat4("projection", projection);
    targetShader.setMat4("view", view);
    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    targetShader.setMat4("model", model);
    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    // render the cube
    glBindVertexArray(targetVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // also draw the lamp object
    sourceShader.use();
    sourceShader.setMat4("projection", projection);
    sourceShader.setMat4("view", view);

    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    sourceShader.setMat4("model", model);

    glBindVertexArray(sourceVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &targetVAO);
  glDeleteVertexArrays(1, &sourceVAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {

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
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

GLFWwindow *InitWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    return NULL;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwSetScrollCallback(window, scroll_callback);
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
