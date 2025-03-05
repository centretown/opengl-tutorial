#include "glad.h"
#include <GLFW/glfw3.h>

#include <cstring>
#include <glm/ext/matrix_transform.hpp>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

// #define MAX_ANGLE 180.0f
// #define MIN_ANGLE 15.0f
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 960
float windowWidth = (float)SCREEN_WIDTH;
float windowHeight = (float)SCREEN_HEIGHT;

// forward decls.
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLFWwindow *InitWindow();
void InitializeLights(Shader &targetShader, Camera &camera);
void UpdateLights(Shader &targetShader, Camera &camera);

// void DrawContainers(float scale);
void DrawContainers(float scale, Shader &shader, unsigned int texture, int VAO);
// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

static float deltaTime = 0.0f;    // Time between current frame and last frame
static float lastFrame = 0.0f;    // Time of last frame
static float currentFrame = 0.0f; // Time of last frame

ShaderCode depthCode("assets/shaders/gls330/depth.vert",
                     "assets/shaders/gls330/depth.frag");

ShaderCode singleCode("assets/shaders/gls330/depth.vert",
                      "assets/shaders/gls330/single.frag");

float rotation_angle = 0.0f;

int main(int argc, char **argv) {

  if (!depthCode.Load()) {
    return -1;
  }

  if (!singleCode.Load()) {
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
  glDepthFunc(GL_LESS);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  Shader curShader(&depthCode);
  if (!curShader.IsValid()) {
    glfwTerminate();
    return -1;
  }
  Shader singleShader(&singleCode);
  if (!singleShader.IsValid()) {
    glfwTerminate();
    return -1;
  }
  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float cubeVertices[] = {
      // positions          // texture Coords
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

  // positions          // texture Coords (note we set these higher than 1
  // (together with GL_REPEAT as texture wrapping mode). this will cause the
  // floor texture to repeat)
  float planeVertices[] = {
      5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, 5.0f,
      0.0f, 0.0f,  -5.0f, -0.5f, -5.0f, 0.0f,  2.0f,

      5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, -5.0f,
      0.0f, 2.0f,  5.0f,  -0.5f, -5.0f, 2.0f,  2.0f};

  unsigned int cubeVAO, cubeVBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);
  // plane VAO
  unsigned int planeVAO, planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);

  // load textures
  // -------------
  TextureOptions options;
  unsigned int cubeTexture =
      MakeTexture("../resources/textures/marble.jpg", &options);
  unsigned int floorTexture =
      MakeTexture("../resources/textures/metal.png", &options);
  curShader.use();
  curShader.setInt("texture1", 0);

  glEnable(GL_DEPTH_TEST);
  // glDepthMask(GL_LESS);
  glEnable(GL_STENCIL_TEST);

  while (!glfwWindowShouldClose(window)) {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    singleShader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    view = glm::rotate(view, glm::radians(rotation_angle), axis);
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), windowWidth / windowHeight, 0.1f, 100.0f);
    glBindVertexArray(cubeVAO);
    singleShader.setMat4("view", view);
    singleShader.setMat4("projection", projection);

    curShader.use();
    curShader.setMat4("view", view);
    curShader.setMat4("projection", projection);

    // floor
    glStencilMask(0x00);
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    curShader.setMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // 1st. render pass, draw objects as normal, writing to the stencil buffer
    // --------------------------------------------------------------------
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    // cubes
    //
    DrawContainers(1.0f, curShader, cubeTexture, cubeVAO);
    // glBindVertexArray(cubeVAO);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, cubeTexture);
    // model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    // curShader.setMat4("model", model);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    // curShader.setMat4("model", model);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    // 2nd. render pass: now draw slightly scaled versions of the objects, this
    // time disabling stencil writing. Because the stencil buffer is now filled
    // with several 1s. The parts of the buffer that are 1 are not drawn, thus
    // only drawing the objects' size differences, making it look like borders.
    // -----------------------------------------------------------------------------------------------------------------------------
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    // singleShader.use();
    // float scale = 1.1f;
    DrawContainers(1.1f, singleShader, cubeTexture, cubeVAO);
    // cubes
    // glBindVertexArray(cubeVAO);
    // glBindTexture(GL_TEXTURE_2D, cubeTexture);
    // model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    // model = glm::scale(model, glm::vec3(scale, scale, scale));
    // singleShader.setMat4("model", model);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    // model = glm::scale(model, glm::vec3(scale, scale, scale));
    // singleShader.setMat4("model", model);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);

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

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    rotation_angle -= 1.0;
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
    rotation_angle += 1.0;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  windowWidth = width;
  windowHeight = height;
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

glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

void InitializeLights(Shader &targetShader, Camera &camera) {

  targetShader.use();
  targetShader.setVec3("viewPos", camera.Position);
  // targetShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
  targetShader.setFloat("material.shininess", 64.0f);

  // point light 1
  targetShader.setVec3("pointLights[0].position", pointLightPositions[0]);
  targetShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[0].constant", 1.0f);
  targetShader.setFloat("pointLights[0].linear", 0.09f);
  targetShader.setFloat("pointLights[0].quadratic", 0.032f);
  // point light 2
  targetShader.setVec3("pointLights[1].position", pointLightPositions[1]);
  targetShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[1].constant", 1.0f);
  targetShader.setFloat("pointLights[1].linear", 0.09f);
  targetShader.setFloat("pointLights[1].quadratic", 0.032f);
  // point light 3
  targetShader.setVec3("pointLights[2].position", pointLightPositions[2]);
  targetShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[2].constant", 1.0f);
  targetShader.setFloat("pointLights[2].linear", 0.09f);
  targetShader.setFloat("pointLights[2].quadratic", 0.032f);
  // point light 4
  targetShader.setVec3("pointLights[3].position", pointLightPositions[3]);
  targetShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[3].constant", 1.0f);
  targetShader.setFloat("pointLights[3].linear", 0.09f);
  targetShader.setFloat("pointLights[3].quadratic", 0.032f);
  // spotLight
  targetShader.setVec3("spotLight.position", camera.Position);
  targetShader.setVec3("spotLight.direction", camera.Front);
  targetShader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
  targetShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
  targetShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("spotLight.constant", 1.0f);
  targetShader.setFloat("spotLight.linear", 0.09f);
  targetShader.setFloat("spotLight.quadratic", 0.032f);
  targetShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
  targetShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}

void UpdateLights(Shader &targetShader, Camera &camera) {
  targetShader.use();
  targetShader.setVec3("viewPos", camera.Position);
  targetShader.setVec3("spotLight.position", camera.Position);
  targetShader.setVec3("spotLight.direction", camera.Front);
}

void DrawContainersx(float scale, Shader &shader, unsigned int texture) {
  // cubes
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glm::mat4 model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
  shader.setMat4("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
  shader.setMat4("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void DrawContainers(float scale, Shader &shader, unsigned int texture,
                    int VAO) {
  glBindVertexArray(VAO);
  shader.use();
  // float scale = 1.1f;
  // cubes
  glBindTexture(GL_TEXTURE_2D, texture);
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
  model = glm::scale(model, glm::vec3(scale, scale, scale));
  shader.setMat4("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
  model = glm::scale(model, glm::vec3(scale, scale, scale));
  shader.setMat4("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}
